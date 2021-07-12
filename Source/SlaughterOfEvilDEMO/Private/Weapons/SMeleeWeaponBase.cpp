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
//#include "Net/UnrealNetwork.h"

// Game Includes
#include "Player/SCharacterBase.h"

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
		MeshComp->SetVisibility(false);
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

	//bIsMagicCharged = false;

	//bReplicates = true;
}




void ASMeleeWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	MeshComp->SetVisibility(false);


	bool IsLocallyControlled = false;
	auto MyOwner = Cast<ASCharacterBase>(GetOwner());
	if (MyOwner)
	{
		IsLocallyControlled = MyOwner->IsLocallyControlled();
		UE_LOG(LogTemp, Warning, TEXT("My Owner is: %s"), *MyOwner->GetName());
	}

	if (MeshComp)
	{
		bool IsServer = GetLocalRole() == ENetRole::ROLE_Authority;
		bool IsFirstPerson = MeleeWeaponPerspective == EMeleeWeaponPerspective::EMWP_FirstPerson;

		if (IsServer && IsFirstPerson && IsLocallyControlled)
		{
			MeshComp->SetVisibility(true);
			MeshComp->SetHiddenInGame(false);
		}
		else if (!IsServer && IsFirstPerson && IsLocallyControlled)
		{
			MeshComp->SetVisibility(true);
			MeshComp->SetHiddenInGame(false);
		}
		else if (!IsServer && !IsFirstPerson && !IsLocallyControlled)
		{
			MeshComp->SetVisibility(true);
			MeshComp->SetHiddenInGame(false);
		}

		/*
		else if (IsServer && !IsFirstPerson && !IsLocallyControlled)
		{
			MeshComp->SetVisibility(true);
		}*/

	

	

		/*if (IsLocallyControlled && MeleeWeaponPerspective == EMeleeWeaponPerspective::EMWP_ThirdPerson)
		{
			MeshComp->SetHiddenInGame(true);
		}*/

		/*/if (MeleeWeaponPerspective == EMeleeWeaponPerspective::EMWP_FirstPerson && IsLocallyControlled)
		{
			MeshComp->SetVisibility(true);
		}
		else if (MeleeWeaponPerspective == EMeleeWeaponPerspective::EMWP_FirstPerson && !IsLocallyControlled)
		{
			MeshComp->SetVisibility(false);
		}
		else if (MeleeWeaponPerspective == EMeleeWeaponPerspective::EMWP_ThirdPerson && IsLocallyControlled)
		{
			MeshComp->SetVisibility(false);
		}
		else if (MeleeWeaponPerspective == EMeleeWeaponPerspective::EMWP_ThirdPerson && !IsLocallyControlled)
		{
			MeshComp->SetVisibility(true);
		}*/
		//else if (MeleeWeaponPerspective == EMeleeWeaponPerspective::EMWP_FirstPerson && !IsLocallyControlled)
		//{
		//	MeshComp->SetVisibility(false);
		//}
		//else if (MeleeWeaponPerspective == EMeleeWeaponPerspective::EMWP_ThirdPerson && IsLocallyControlled)
		//{

		//}
		//else // Third Person
		//{
		//	MeshComp->SetOwnerNoSee(true);
		//	MeshComp->SetOnlyOwnerSee(false);
		//}
	}

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


//bool ASMeleeWeaponBase::TrySetMagicCharge(bool Charged)
//{
//	if (GetLocalRole() == ENetRole::ROLE_Authority)
//	{
//		if (Charged && !bIsMagicCharged)
//		{
//			bIsMagicCharged = true;
//			OnRep_SetMagicCharge();
//			return true;
//		}
//		else if (!Charged && bIsMagicCharged)
//		{
//			bIsMagicCharged = false;
//			OnRep_SetMagicCharge();
//			return true;
//		}
//	}
//
//	return false;
//}


//void ASMeleeWeaponBase::OnRep_SetMagicCharge()
//{
//	auto MyOwner = GetOwner();
//	if (MyOwner)
//	{
//		auto CharacterOwner = Cast<ASCharacterBase>(MyOwner);
//		if (CharacterOwner)
//		{
//			CharacterOwner->SetWeaponMagicCharged(bIsMagicCharged);
//		}
//	}
//}


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
				CollisionComp->SetHiddenInGame(false);
			}
		}
		else if (NewMeleeWeaponState == EMeleeWeaponState::EMWS_Idle)
		{
			if (CollisionComp)
			{
				CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				CollisionComp->SetHiddenInGame(true);
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


//bool ASMeleeWeaponBase::IsMagicCharged() const
//{
//	return bIsMagicCharged;
//}


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


void ASMeleeWeaponBase::SetMeleeWeaponPerspective(EMeleeWeaponPerspective Perspective)
{

	MeleeWeaponPerspective = Perspective;


}


//void ASMeleeWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//	DOREPLIFETIME(ASMeleeWeaponBase, bIsMagicCharged);
//
//}



