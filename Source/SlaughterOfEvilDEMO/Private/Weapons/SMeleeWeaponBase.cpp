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



// Game Includes

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
	}

	bIsMagicCharged = false;
}




void ASMeleeWeaponBase::BeginPlay()
{
	Super::BeginPlay();


	CacheDamageTraceArguments();
}





void ASMeleeWeaponBase::CacheDamageTraceArguments()
{
	if (CollisionComp)
	{
		TraceRadius = CollisionComp->GetScaledCapsuleRadius();
		TraceHalfHeight = CollisionComp->GetScaledCapsuleHalfHeight();
	}

	TraceIgnoreActors.Add(GetOwner());
	CollisionObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
}

void ASMeleeWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if ((GetLocalRole() == ENetRole::ROLE_Authority) && (MeleeWeaponState == EMeleeWeaponState::EMWS_Attacking))
	{
		bool Hit = CheckForCollision();
	}
}

bool ASMeleeWeaponBase::CheckForCollision()
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
			CollisionObjectTypes,
			true,
			TraceIgnoreActors,
			EDrawDebugTrace::None,
			HitResult,
			true
		);

		if (Hit)
		{
			DrawDebugSphere(GetWorld(), HitResult.Location, 5.f, 12, FColor::Red, false, 2.f, 0.f, 0.3f);
			TrySetMeleeWeaponState(EMeleeWeaponState::EMWS_Idle);
		}
		else
		{
			PreviousCollisionCenter = TraceEnd;
		}
	}

	return true;
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
		if (MeleeWeaponState != EMeleeWeaponState::EMWS_Attacking && NewMeleeWeaponState == EMeleeWeaponState::EMWS_Attacking)
		{
			if (CollisionComp)
			{
				PreviousCollisionCenter = CollisionComp->GetComponentLocation();
				UE_LOG(LogTemp, Warning, TEXT("Start Sphere Trace"));
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


bool ASMeleeWeaponBase::IsMagicCharged() const
{
	return bIsMagicCharged;
}


EMeleeWeaponState ASMeleeWeaponBase::GetMeleeWeaponState() const
{
	return MeleeWeaponState;
}

void ASMeleeWeaponBase::SetCanCauseDamage(bool CanDamage)
{
	CanDamage ? TrySetMeleeWeaponState(EMeleeWeaponState::EMWS_Attacking) : TrySetMeleeWeaponState(EMeleeWeaponState::EMWS_Idle);
}


void ASMeleeWeaponBase::SetIsBlocking(bool Blocking)
{
	Blocking ? TrySetMeleeWeaponState(EMeleeWeaponState::EMWS_Blocking) : TrySetMeleeWeaponState(EMeleeWeaponState::EMWS_Idle);
}



