// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/SMeleeWeaponBase.h"

// Engine Includes
#include "DrawDebugHelpers.h"
#include "Engine/CollisionProfile.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Net/UnrealNetwork.h"


// Game Includes
#include "Player/SCharacterBase.h"
#include "Components/SMagicChargeComponent.h"

// Sets default values
ASMeleeWeaponBase::ASMeleeWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	if (RootComp)
	{
		SetRootComponent(RootComp);
	}

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	if (MeshComp && RootComp)
	{
		MeshComp->SetupAttachment(RootComp);
	}

	CollisionComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionComp"));
	if (CollisionComp)
	{
		CollisionComp->SetupAttachment(GetRootComponent());
		CollisionComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
		CollisionComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
		CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	MagicChargeComp = CreateDefaultSubobject<USMagicChargeComponent>(TEXT("MagicChargeComp"));

	bWeaponVisibility = true;

	bReplicates = true;
}


void ASMeleeWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (MeshComp)
	{
		MeshComp->SetVisibility(bWeaponVisibility);
	}

	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		if (MagicChargeComp)
		{
			MagicChargeComp->OnMagicChargeChange.AddDynamic(this, &ASMeleeWeaponBase::OnMagicChargeChange);
		}

		CacheDamageTraceArguments();
	}


	
}




void ASMeleeWeaponBase::CacheDamageTraceArguments()
{
	if (CollisionComp)
	{
		TraceRadius = CollisionComp->GetScaledCapsuleRadius();
		TraceHalfHeight = CollisionComp->GetScaledCapsuleHalfHeight();
	}

	DamageTraceIgnoreActors.Add(GetOwner());
	DamageCollisionObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
}


void ASMeleeWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if ((GetLocalRole() == ENetRole::ROLE_Authority) && (MeleeWeaponState == EMeleeWeaponState::EMWS_Attacking))
	{
		bool Hit = CheckForAttackTraceCollision();
	}
}





bool ASMeleeWeaponBase::CheckForAttackTraceCollision()
{
	if (CollisionComp)
	{

		FVector TraceEnd = CollisionComp->GetComponentLocation();

		bool Hit = UKismetSystemLibrary::CapsuleTraceSingleForObjects(
			this,
			PreviousCollisionCenter,
			TraceEnd,
			TraceRadius,
			TraceHalfHeight,
			DamageCollisionObjectTypes,
			true,
			DamageTraceIgnoreActors,
			EDrawDebugTrace::None,
			HitResult,
			true
		);

		if (Hit)
		{
			DrawDebugSphere(GetWorld(), HitResult.Location, 5.f, 12, FColor::Red, false, 2.f, 0.f, 0.3f);
			//TrySetMeleeWeaponState(EMeleeWeaponState::EMWS_Idle);
		}
		else
		{
			PreviousCollisionCenter = TraceEnd;
		}
	}

	return true;
}


void ASMeleeWeaponBase::OnMagicChargeChange(bool Value)
{
	auto MyOwner = GetOwner();
	if (MyOwner)
	{
		auto MeleeWeaponWielder = Cast<ISMeleeWeaponWielder>(MyOwner);
		if (MeleeWeaponWielder)
		{
			MeleeWeaponWielder->WeaponMagicChargeChange(Value);
		}
	}
}


bool ASMeleeWeaponBase::TrySetMeleeWeaponState(EMeleeWeaponState NewMeleeWeaponState)
{

	//
	// Apply and state change conditions here
	//
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		if (MeleeWeaponState != EMeleeWeaponState::EMWS_Attacking && NewMeleeWeaponState == EMeleeWeaponState::EMWS_Attacking)
		{
			if (CollisionComp)
			{
				PreviousCollisionCenter = CollisionComp->GetComponentLocation();
				UE_LOG(LogTemp, Warning, TEXT("Start Sphere Trace"));
				CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}
		else if (NewMeleeWeaponState == EMeleeWeaponState::EMWS_Blocking)
		{
			// Enable Collisions to block
			if (CollisionComp)
			{
				CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			}
		}
		else if (NewMeleeWeaponState == EMeleeWeaponState::EMWS_Idle)
		{
			if (CollisionComp)
			{
				CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}

		else if (NewMeleeWeaponState != EMeleeWeaponState::EMWS_Attacking)
		{
			UE_LOG(LogTemp, Warning, TEXT("Stop Sphere Trace"));
		}

		MeleeWeaponState = NewMeleeWeaponState;
		return true;
	}

	return false;
}


EMeleeWeaponState ASMeleeWeaponBase::GetMeleeWeaponState() const
{
	return MeleeWeaponState;
}


bool ASMeleeWeaponBase::GetIsWeaponMagicCharged()
{
	return (MagicChargeComp) ? MagicChargeComp->IsMagicCharged() : false;
}


bool ASMeleeWeaponBase::SetCanCauseDamage(bool CanDamage)
{
	if (CanDamage)
	{
		return  TrySetMeleeWeaponState(EMeleeWeaponState::EMWS_Attacking) && MeleeWeaponState == EMeleeWeaponState::EMWS_Attacking;
	}
	else
	{
		return TrySetMeleeWeaponState(EMeleeWeaponState::EMWS_Idle) && MeleeWeaponState == EMeleeWeaponState::EMWS_Idle;
	}
}


bool ASMeleeWeaponBase::SetIsBlocking(bool Blocking)
{
	if (Blocking)
	{
		return TrySetMeleeWeaponState(EMeleeWeaponState::EMWS_Blocking) && MeleeWeaponState == EMeleeWeaponState::EMWS_Blocking;
	}
	else
	{
		return TrySetMeleeWeaponState(EMeleeWeaponState::EMWS_Idle) && MeleeWeaponState == EMeleeWeaponState::EMWS_Idle;
	}

}


#pragma region Weapon Setup

void ASMeleeWeaponBase::SetOwningPawn(ASCharacterBase* NewOwner)
{
	if (NewOwner != MyPawn)
	{
		SetInstigator(NewOwner);
		MyPawn = NewOwner;
		SetOwner(NewOwner);
	}
}


void ASMeleeWeaponBase::OnRep_MyPawn()
{
	if (MyPawn)
	{
		OnEnterInventory(MyPawn);
	}
	else
	{
		OnLeaveInventory();
	}
}


void ASMeleeWeaponBase::OnEnterInventory(ASCharacterBase* NewOwner)
{
	SetOwningPawn(NewOwner);
}


void ASMeleeWeaponBase::OnLeaveInventory()
{

}


void ASMeleeWeaponBase::OnEquip()
{
	AttachMeshToPawn();
}


void ASMeleeWeaponBase::AttachMeshToPawn()
{
	if (!MyPawn) return;

	GetRootComponent()->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	MeshComp->SetHiddenInGame(true);

	FName AttackSocketName = MyPawn->GetWeaponSocketName();
	if (MyPawn->IsLocallyControlled())
	{
		if (GetLocalRole() != ENetRole::ROLE_Authority)
		{
			UE_LOG(LogTemp, Warning, TEXT("AttachMeshToPawn, not server"));
		}
		auto MyPawnFPP = MyPawn->GetFirstPersonMesh();
		if (MyPawnFPP)
		{
			MeshComp->SetHiddenInGame(false);
			GetRootComponent()->AttachToComponent(MyPawnFPP, FAttachmentTransformRules::KeepRelativeTransform, AttackSocketName);

		}
	}
	else
	{
		auto MyPawnTPP = MyPawn->GetThirdPersonMesh();
		if (MyPawnTPP)
		{
			MeshComp->SetHiddenInGame(false);
			GetRootComponent()->AttachToComponent(MyPawnTPP, FAttachmentTransformRules::KeepRelativeTransform, AttackSocketName);
		}
	}
}

#pragma endregion

void ASMeleeWeaponBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASMeleeWeaponBase, MyPawn);


}