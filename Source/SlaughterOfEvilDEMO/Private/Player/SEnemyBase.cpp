// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SEnemyBase.h"

// Engine Includes
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

// Game Includes
#include "AI/AIGroupControlComponent.h"
#include "Magic/SMagicProjectileBase.h"
#include "Weapons/SMeleeWeaponBase.h"
#include "Components/SphereComponent.h"


const static int32 NO_VALID_ATTACK_IN_RANGE = -1;



ASEnemyBase::ASEnemyBase()
{
	AIGroupControlComp = CreateDefaultSubobject<UAIGroupControlComponent>(TEXT("AIGroupControlComp"));
	AIGroupSphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("AIGroupSphereComp"));
	if (AIGroupSphereComp)
	{
		AIGroupSphereComp->SetupAttachment(GetRootComponent());
		AIGroupSphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		AIGroupSphereComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AIGroupSphereComp->SetSphereRadius(150.f);
	
	}
}


void ASEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ENetRole::ROLE_Authority && AIGroupControlComp)
	{
		AIGroupControlComp->OnTriggerAttack.AddDynamic(this, &ASEnemyBase::AIGroupControllerTriggerMeleeAttack);
		AIGroupControlComp->OnTriggerRangeAttack.AddDynamic(this, &ASEnemyBase::AIGroupControllerTriggerRangeAttack);
	}

}


void ASEnemyBase::AIGroupControllerTriggerMeleeAttack(AActor* GroupTarget)
{
	UE_LOG(LogTemp, Warning, TEXT("AIGroupControllerTriggerMeleeAttack"));
	
	UseMeleeAttackInRangeOfGroupTarget();
}


void ASEnemyBase::UseMeleeAttackInRangeOfGroupTarget()
{
	ASMeleeWeapon* MeleeWeapon = GetCurrentMeleeWeapon();
	if (MeleeWeapon)
	{
		float DistanceToTarget = FVector::Distance(GetActorLocation(), AIGroupControlComp->GroupControllerData.TargetActor->GetActorLocation());
		TArray<FUseWeaponAnims> MeleeWeaponAttacks = MeleeWeapon->GetWeaponDataAttackAnims();
		TArray<int32> ValidMeleeWeaponAttackIndexs;
		for (int32 i = 0; i < MeleeWeaponAttacks.Num(); ++i)
		{
			if (DistanceToTarget > MeleeWeaponAttacks[i].MinDistanceToUse && DistanceToTarget < MeleeWeaponAttacks[i].MaxDistanceToUse)
			{
				ValidMeleeWeaponAttackIndexs.Add(i);
			}
		}

		if (ValidMeleeWeaponAttackIndexs.Num() > 0)
		{
			MeleeWeapon->UseSpecificMeleeAttack(FMath::RandRange(0, ValidMeleeWeaponAttackIndexs.Num() - 1));
		}
	}
}


void ASEnemyBase::AIGroupControllerTriggerRangeAttack(AActor* GroupTarget)
{
	//UE_LOG(LogTemp, Warning, TEXT("AIGroupControllerTriggerRangeAttack"));
	//if (MagicUseState == EMagicUseState::EMUS_Idle)
	//{
	//	ProjectileMagicAttack();
	//}
}

//void ASEnemyBase::MeleeAttack()
//{
//	if (GetLocalRole() != ENetRole::ROLE_Authority) return;
//
//	if (MeleeAttackState == EMeleeAttackState::EMAS_Idle)
//	{
//		TrySetMeleeAttackState(EMeleeAttackState::EMAS_Attacking);
//		int32 MeleeAttackIndex = FindMeleeAttackInRangeOfGroupTarget();
//		if (MeleeAttackIndex != NO_VALID_ATTACK_IN_RANGE)
//		{
//			AIGroupControlComp->TriggerAttackUpdate();
//			MulticastPlayMeleeAttackMontage(MeleeAttackIndex);
//		}
//	}
//}




void ASEnemyBase::ProjectileMagicAttack()
{
	//CurrentProjectMagicCastIndex = FindValidProjectileMagicAttackIndex();
	//if (CurrentProjectMagicCastIndex != 1)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Will Crash: index = %d"), CurrentProjectMagicCastIndex);
	//}
	//if (CurrentProjectMagicCastIndex != NO_VALID_ATTACK_IN_RANGE)
	//{

	//	MulticastPlayProjectileMagicAttackMontage(CurrentProjectMagicCastIndex);
	//}
}


void ASEnemyBase::SpawnMagicAttackProjectile()
{
	//if (CurrentProjectMagicCastIndex == -1) return;
	//FTransform MagicAttackPrjectileTransform;
	//FVector ProjectileTargetLocation;
	//UE_LOG(LogTemp, Warning, TEXT("Will Crash in SpawnMagicAttackProjectile: index = %d"), CurrentProjectMagicCastIndex);
	//if (!ProjectileMagicCasts[CurrentProjectMagicCastIndex].SpawnSocket.IsNone() &&
	//	GetMesh()->DoesSocketExist(ProjectileMagicCasts[CurrentProjectMagicCastIndex].SpawnSocket))
	//{
	//	FVector Location;
	//	FQuat Quat;
	//	GetMesh()->GetSocketWorldLocationAndRotation(ProjectileMagicCasts[CurrentProjectMagicCastIndex].SpawnSocket, Location, Quat);
	//	MagicAttackPrjectileTransform.SetRotation(Quat);
	//	MagicAttackPrjectileTransform.SetLocation(Location);
	//}
	//else
	//{
	//	MagicAttackPrjectileTransform.SetLocation(GetActorLocation());
	//	MagicAttackPrjectileTransform.SetRotation(GetActorQuat());
	//}

	//if (AIGroupControlComp)
	//{
	//	ProjectileTargetLocation = AIGroupControlComp->GroupControllerData.TargetActor->GetActorLocation();
	//}
	//else
	//{
	//	ProjectileTargetLocation = GetActorLocation() + GetActorForwardVector();
	//}

	//auto LookAtRotation = UKismetMathLibrary::FindLookAtRotation(MagicAttackPrjectileTransform.GetLocation(), ProjectileTargetLocation);
	//LookAtRotation.Pitch = 0.f;
	//MagicAttackPrjectileTransform.SetRotation(LookAtRotation.Quaternion());
	//

	//
	//ASMagicProjectileBase* Projectile = Cast<ASMagicProjectileBase>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, ProjectileMagicCasts[CurrentProjectMagicCastIndex].MagicProjectile, MagicAttackPrjectileTransform, ESpawnActorCollisionHandlingMethod::AlwaysSpawn));
	//if (Projectile)
	//{
	//	Projectile->SetOwner(this);
	//	Projectile->SetInstigator(this);
	//	//Projectile->InitialSpeed = 1000.f;
	//	//Projectile->Mass = 1.f;
	//	//Projectile->Drag = 0.f;
	//	//Projectile->Gravity = FVector::ZeroVector;

	//	if (AIGroupControlComp)
	//	{
	//		Projectile->SetHomingTargetActor(AIGroupControlComp->GroupControllerData.TargetActor);
	//	}

	//	UGameplayStatics::FinishSpawningActor(Projectile, MagicAttackPrjectileTransform);
	//}
}


int32 ASEnemyBase::FindValidProjectileMagicAttackIndex()
{
	/*if (!AIGroupControlComp) return NO_VALID_ATTACK_IN_RANGE;

	FVector Location = GetActorLocation();
	FVector TargetLocation = AIGroupControlComp->GroupControllerData.TargetActor->GetActorLocation();

	float DistanceToTarget = FVector::Distance(TargetLocation, Location);
	UE_LOG(LogTemp, Warning, TEXT("Distance To target is: %f"), DistanceToTarget);

	TArray<int32> PossibleAttackIndex;

	for (int32 AttackIndex = 0; AttackIndex < ProjectileMagicCasts.Num(); ++AttackIndex)
	{
		if (DistanceToTarget >= ProjectileMagicCasts[AttackIndex].MinDistance &&
			DistanceToTarget <= ProjectileMagicCasts[AttackIndex].MaxDistance &&
			IsProjectileViewUnobstructed(Location, TargetLocation, 100.f))
		{
			PossibleAttackIndex.Add(AttackIndex);
		}
	}

	if (PossibleAttackIndex.Num() < 1)
	{
		return NO_VALID_ATTACK_IN_RANGE;
	}
	else
	{
		int32 RandomIndex = FMath::RandRange(0, PossibleAttackIndex.Num() - 1);
		return PossibleAttackIndex[RandomIndex];
	}*/

	return 0;
}

bool ASEnemyBase::IsProjectileViewUnobstructed(FVector& ProjectileLaunchPosition, FVector& ProjectileTargetPosition, float Radius)
{
	TArray<AActor*> IgnoreActors;
	FHitResult HitResult;
	bool Hit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), ProjectileLaunchPosition, ProjectileLaunchPosition, Radius, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, IgnoreActors, EDrawDebugTrace::None, HitResult, true);
	return Hit;
}

