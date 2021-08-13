// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/SMeleeWeapon.h"
// Engine Includes
#include "DrawDebugHelpers.h"
#include "Engine/CollisionProfile.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystem.h"

#include "Components/SMagicChargeComponent.h"
#include "Player/SCharacterBase.h"

static const float ANIM_PLAY_FAIL = 0.f;

ASMeleeWeapon::ASMeleeWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	MagicChargeCollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("MagicChargeCollisionComp"));
	if (MagicChargeCollisionComp)
	{
		//MagicChargeCollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		MagicChargeCollisionComp->SetupAttachment(GetRootComponent());
		MagicChargeCollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		MagicChargeCollisionComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
		MagicChargeCollisionComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		MagicChargeCollisionComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	}

	CosmeticCollisionPoint = CreateDefaultSubobject<USceneComponent>(TEXT("CosmeticCollisionPoint"));
	if (CosmeticCollisionPoint && MeshComp)
	{
		CosmeticCollisionPoint->SetupAttachment(MeshComp);
	}
}


void ASMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		if (MagicChargeComp)
		{
			MagicChargeComp->SetCanMagicCharge(MeleeWeaponData.bCanMagicBlock);
		}
	}
}


void ASMeleeWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bCosmeticTraceEnable)
	{
		DisplayCosmeticVFX();
	}
}


/*************************************************************************/
/* FX */
/*************************************************************************/
bool ASMeleeWeapon::CosmeticCollisionTrace(FHitResult& HitResult)
{
	bool bHit = false;
	if (CosmeticCollisionPoint)
	{
		FVector CurrentCosmeticTraceCenter = CosmeticCollisionPoint->GetComponentLocation();

		ETraceTypeQuery TraceTypeQuery = UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel2);
		bHit = UKismetSystemLibrary::SphereTraceSingle(
			this,
			PreviousCosmeticTraceCenter,
			CurrentCosmeticTraceCenter,
			TraceRadius,
			TraceTypeQuery,
			true,
			CosmeticTraceIgnoreActors,
			EDrawDebugTrace::ForOneFrame,
			HitResult,
			true
		);
		PreviousCosmeticTraceCenter = CurrentCosmeticTraceCenter;
	}

	return bHit;
}


void ASMeleeWeapon::DisplayCosmeticVFX()
{
	FHitResult HitResult;
	bool Hit = CosmeticCollisionTrace(HitResult);
	if (Hit && FleshCollisionVFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FleshCollisionVFX, HitResult.ImpactPoint, (HitResult.ImpactNormal * -1.f).Rotation());
		bCosmeticTraceEnable = false;
	}
}


void ASMeleeWeapon::EnableCosmeticWeaponTrace(bool Enable)
{
	if (Enable && UseState == EMeleeWeaponUseState::EMWUS_Attacking)
	{
		//EnableCollisionsCosmeticCollisionComp(true);
		bCosmeticTraceEnable = true;
		PreviousCosmeticTraceCenter = CosmeticCollisionPoint->GetComponentLocation();
		CosmeticTraceIgnoreActors.AddUnique(this);
		CosmeticTraceIgnoreActors.AddUnique(MyPawn);
	}
	else
	{
		//EnableCollisionsCosmeticCollisionComp(false);
		bCosmeticTraceEnable = false;
	}
}


/*************************************************************************/
/* Melee Attack (Use) */
/*************************************************************************/
void ASMeleeWeapon::Use()
{
	if (WeaponIsIdle())
	{
		if (GetLocalRole() < ENetRole::ROLE_Authority)
		{
			ServerUse();
		}

		IncrementMeleeAttackCount();

		if (MeleeWeaponData.Attacks.Num() > MeleeAttackCount)
		{
			TriggerOwnerPlayAttackAnimation(MeleeWeaponData.Attacks[MeleeAttackCount]);

			if (GetLocalRole() == ENetRole::ROLE_Authority)
			{
				if (PlayAnimMontageOnServer(MeleeWeaponData.Attacks[MeleeAttackCount].FirstPerson) > ANIM_PLAY_FAIL)
				{
					TrySetUseState(EMeleeWeaponUseState::EMWUS_Attacking);
				}
			}
		}
	}
}


void ASMeleeWeapon::ServerUse_Implementation()
{
	Use();
}


void ASMeleeWeapon::IncrementMeleeAttackCount()
{
	if (MeleeWeaponData.Attacks.Num() > 0)
	{
		MeleeAttackCount = ++MeleeAttackCount % MeleeWeaponData.Attacks.Num();
	}
}


void ASMeleeWeapon::OnUseFinish()
{
	TrySetUseState(EMeleeWeaponUseState::EMWUS_Idle);
}


void ASMeleeWeapon::OnRep_MeleeAttackCount()
{
	if (MyPawn && MyPawn->IsLocallyControlled() == false)
	{
		TriggerOwnerPlayAttackAnimation(MeleeWeaponData.Attacks[MeleeAttackCount]);
	}
}


/*************************************************************************/
/* Melee Block */
/*************************************************************************/
void ASMeleeWeapon::StartBlock()
{
	if (WeaponIsIdle())
	{
		if (GetLocalRole() < ENetRole::ROLE_Authority)
		{
			ServerStartBlock();
		}

		if (!bIsBlocking)
		{
			bIsBlocking = true;
			TriggerOwnerPlayAttackAnimation(MeleeWeaponData.StartBlockAnims);
			if (MeleeWeaponData.bCanMagicBlock)
			{
				EnableMagicChargeCollisions(true);
			}
		}
	}
}


void ASMeleeWeapon::ServerStartBlock_Implementation()
{
	StartBlock();
}


void ASMeleeWeapon::StopBlock()
{
	if (GetLocalRole() < ENetRole::ROLE_Authority)
	{
		ServerStopBlock();
	}

	if (bIsBlocking)
	{
		bIsBlocking = false;
		TriggerOwnerPlayAttackAnimation(MeleeWeaponData.StopBlockAnims);
		if (MeleeWeaponData.bCanMagicBlock)
		{
			EnableMagicChargeCollisions(false);
		}
	}
}


void ASMeleeWeapon::ServerStopBlock_Implementation()
{
	StopBlock();
}


void ASMeleeWeapon::OnRep_IsBlocking()
{
	if (MyPawn->IsLocallyControlled() == false)
	{
		bIsBlocking ? TriggerOwnerPlayAttackAnimation(MeleeWeaponData.StartBlockAnims) : TriggerOwnerPlayAttackAnimation(MeleeWeaponData.StopBlockAnims);
	}
}


/*************************************************************************/
/* Magic */
/*************************************************************************/
void ASMeleeWeapon::StartUseMagicCharge()
{
	if (WeaponIsIdle())
	{
		if (GetLocalRole() < ENetRole::ROLE_Authority)
		{
			ServerStartUseMagicCharge();
		}

		if (bIsMagicCharged && ChargeState == EMeleeWeaponChargeState::EMWCS_Idle)
		{
			TriggerOwnerPlayAttackAnimation(MeleeWeaponData.MagicChargeAttack.Start);
			if (GetLocalRole() == ENetRole::ROLE_Authority)
			{
				if (PlayAnimMontageOnServer(MeleeWeaponData.MagicChargeAttack.Start.FirstPerson) > ANIM_PLAY_FAIL)
				{
					TrySetChargeState(EMeleeWeaponChargeState::EMWCS_Charging);
				}
			}
		}
	}
}


void ASMeleeWeapon::ServerStartUseMagicCharge_Implementation()
{
	StartUseMagicCharge();
}



void ASMeleeWeapon::OnMagicChargeReady()
{
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		TrySetChargeState(EMeleeWeaponChargeState::EMWCS_Charged);
	}
}


void ASMeleeWeapon::FinishUseMagicCharge()
{
	if (GetLocalRole() < ENetRole::ROLE_Authority)
	{
		ServerFinishMagicCharge();
	}

	if (bIsMagicCharged && ChargeState == EMeleeWeaponChargeState::EMWCS_Charged)
	{
		TriggerOwnerPlayAttackAnimation(MeleeWeaponData.MagicChargeAttack.Finish);
		if (GetLocalRole() == ENetRole::ROLE_Authority)
		{
			if (PlayAnimMontageOnServer(MeleeWeaponData.MagicChargeAttack.Finish.FirstPerson) > ANIM_PLAY_FAIL)
			{
				TrySetChargeState(EMeleeWeaponChargeState::EMWCS_Finish);
			}
		}
	}
}


void ASMeleeWeapon::ServerFinishMagicCharge_Implementation()
{
	FinishUseMagicCharge();
}


void ASMeleeWeapon::OnMagicChargeRelease()
{
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		if (bIsMagicCharged && ChargeState == EMeleeWeaponChargeState::EMWCS_Finish)
		{
			TrySetChargeState(EMeleeWeaponChargeState::EMWCS_Idle);

			if (MyPawn && MagicChargeComp && MagicChargeComp->TryRemoveMagicCharge())
			{
				ReleaseMagicCharge(MyPawn->GetMagicLaunchTransform());
			}
		}
	}
}


bool ASMeleeWeapon::GetIsWeaponMagicChargeReady() const
{
	return (ChargeState == EMeleeWeaponChargeState::EMWCS_Charged);
}


void ASMeleeWeapon::OnRep_ChargeState()
{
	if (MyPawn && MyPawn->IsLocallyControlled() == false)
	{
		if (ChargeState == EMeleeWeaponChargeState::EMWCS_Charging)
		{
			TriggerOwnerPlayAttackAnimation(MeleeWeaponData.MagicChargeAttack.Start);
		}
		else if (ChargeState == EMeleeWeaponChargeState::EMWCS_Finish)
		{
			TriggerOwnerPlayAttackAnimation(MeleeWeaponData.MagicChargeAttack.Finish);
		}
	}
}


void ASMeleeWeapon::OnMagicChargeStateChange(bool IsMagicCharged)
{
	if (GetLocalRole() != ENetRole::ROLE_Authority) return;
	bIsMagicCharged = IsMagicCharged;
	OnRep_IsMagicCharged();
}


void ASMeleeWeapon::EnableMagicChargeCollisions(bool Enable)
{
	if (MagicChargeCollisionComp)
	{
		Enable ? MagicChargeCollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly) : MagicChargeCollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}



/*************************************************************************/
/* Melee Weapon State */
/*************************************************************************/
bool ASMeleeWeapon::TrySetUseState(EMeleeWeaponUseState NewMeleeWeaponUseState)
{
	//
	// Apply and state change conditions here
	//
	bool ValidStateChange = false;
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		if (UseState != EMeleeWeaponUseState::EMWUS_Attacking && NewMeleeWeaponUseState == EMeleeWeaponUseState::EMWUS_Attacking ||
			UseState == EMeleeWeaponUseState::EMWUS_Idle && NewMeleeWeaponUseState == EMeleeWeaponUseState::EMWUS_Blocking ||
			NewMeleeWeaponUseState == EMeleeWeaponUseState::EMWUS_Idle)
		{
			ValidStateChange = true;
		}

		if (ValidStateChange)
		{
			UseState = NewMeleeWeaponUseState;
		}
	}
	return ValidStateChange;
}


bool ASMeleeWeapon::TrySetChargeState(EMeleeWeaponChargeState NewMeleeWeaponChargeState)
{
	bool ValidStateChange = false;
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		if ((bIsMagicCharged && ChargeState == EMeleeWeaponChargeState::EMWCS_Idle && NewMeleeWeaponChargeState == EMeleeWeaponChargeState::EMWCS_Charging) ||
			(bIsMagicCharged && ChargeState == EMeleeWeaponChargeState::EMWCS_Charging && NewMeleeWeaponChargeState == EMeleeWeaponChargeState::EMWCS_Charged) ||
			(ChargeState == EMeleeWeaponChargeState::EMWCS_Charged && NewMeleeWeaponChargeState == EMeleeWeaponChargeState::EMWCS_Finish) ||
			NewMeleeWeaponChargeState == EMeleeWeaponChargeState::EMWCS_Idle)
		{
			ValidStateChange = true;
		}

		if (ValidStateChange)
		{
			ChargeState = NewMeleeWeaponChargeState;
		}
	}

	return ValidStateChange;
}


bool ASMeleeWeapon::WeaponIsIdle() const
{
	return UseState == EMeleeWeaponUseState::EMWUS_Idle;
}


bool ASMeleeWeapon::WeaponIsAttacking() const
{
	return UseState == EMeleeWeaponUseState::EMWUS_Attacking;
}


bool ASMeleeWeapon::WeaponIsBlocking() const
{
	return UseState == EMeleeWeaponUseState::EMWUS_Blocking;
}


/*************************************************************************/
/* Damage */
/*************************************************************************/
void ASMeleeWeapon::TryApplyDamage()
{
	if (GetLocalRole() == ENetRole::ROLE_Authority && WeaponIsAttacking())
	{
		FVector ApplyDamageCenter = MyPawn->GetActorLocation() + MyPawn->GetActorForwardVector() * MeleeWeaponData.ApplyDamageForwardOffset;
		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(this);
		IgnoreActors.Add(MyPawn);
		TArray<AActor*> OverlapActors;
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));

		UKismetSystemLibrary::SphereOverlapActors(GetWorld(), ApplyDamageCenter, MeleeWeaponData.ApplyDamageRadius, ObjectTypes, nullptr,
			IgnoreActors, OverlapActors);

		if (MeleeWeaponData.bDrawDebugDamageSphere)
		{
			DrawDebugSphere(GetWorld(), ApplyDamageCenter, MeleeWeaponData.ApplyDamageRadius, 15, FColor::Red, false, 3.f, 0, 3.f);
		}

		for (auto& Actor : OverlapActors)
		{
			// Apply Damage
		}
	}
}


/*************************************************************************/
/* Helpers */
/*************************************************************************/



/*************************************************************************/
/* Replication */
/*************************************************************************/
void ASMeleeWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASMeleeWeapon, MeleeAttackCount);
	DOREPLIFETIME(ASMeleeWeapon, UseState);
	DOREPLIFETIME(ASMeleeWeapon, ChargeState);

	DOREPLIFETIME(ASMeleeWeapon, bIsBlocking);

}

