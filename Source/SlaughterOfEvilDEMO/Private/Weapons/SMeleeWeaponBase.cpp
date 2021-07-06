// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/SMeleeWeaponBase.h"

// Engine Includes
#include "DrawDebugHelpers.h"
#include "Engine/CollisionProfile.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"



// Game Includes

// Sets default values
ASMeleeWeaponBase::ASMeleeWeaponBase()
{

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

	bIsMagicCharged = false;
}


void ASMeleeWeaponBase::BeginPlay()
{
	//MyOwner = GetOwner();
}


bool ASMeleeWeaponBase::SetMagicChargeState(bool Charged)
{
	// Prevent changing bMagicCharged to same value
	if ((!bIsMagicCharged && Charged) || (bIsMagicCharged && !Charged))
	{
		bIsMagicCharged = Charged;
		return true;
	}
	
	return false;
}


bool ASMeleeWeaponBase::TrySetMeleeWeaponState(EMeleeWeaponState NewMeleeWeaponState)
{

	//
	// Apply and state change conditions here
	//

	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		MeleeWeaponState = NewMeleeWeaponState;
		return true;
	}

	return false;
}

bool ASMeleeWeaponBase::IsMagicCharged() const
{
	return bIsMagicCharged;
}


EMeleeWeaponState ASMeleeWeaponBase::GetMeleeWeaponState() const
{
	return MeleeWeaponState;
}

bool ASMeleeWeaponBase::TryApplyDamage(FVector& ApplyDamageCenter) const
{
	if (GetOwner())
	{
		/*FVector DamageSphereLocation = GetOwner()->GetActorLocation();
		DamageSphereLocation.Z += ApplyDamageData.DamageSphereVerticleOffset;
		DamageSphereLocation += GetOwner()->GetActorForwardVector() * ApplyDamageData.DamageSphereOwnerForwardOffset;*/

		TArray<AActor*> OverlappingActors;
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(GetOwner());

		if (bDrawDebugHitSphere)
		{
			DrawDebugSphere(GetWorld(), ApplyDamageCenter, ApplyDamageData.DamageSphereRadius, 12, FColor::Red, false, 1.f, 0.f, 1.f);
		}

		bool hitActor = UKismetSystemLibrary::SphereOverlapActors(
			GetWorld(),
			ApplyDamageCenter,
			ApplyDamageData.DamageSphereRadius,
			OverlappingObjectTypes,
			nullptr,
			ActorsToIgnore,
			OverlappingActors);
		
		// Apply damage to all hit actors
		if (hitActor)
		{
			for (auto Actor : OverlappingActors)
			{
				APawn* OwnerPawn = Cast<APawn>(GetOwner());
				if (OwnerPawn)
				{
					AController* OwnerController = OwnerPawn->GetController();
					if (OwnerController)
					{
						UGameplayStatics::ApplyDamage(Actor, ApplyDamageData.BaseDamage, OwnerController, GetOwner(), ApplyDamageData.DamageTypeClass);
					}
				}
			}
		}

		return hitActor;
	}

	return false;
}

void ASMeleeWeaponBase::SetApplyDamageData(FApplyDamageData DamageData)
{
	ApplyDamageData = DamageData;
}

