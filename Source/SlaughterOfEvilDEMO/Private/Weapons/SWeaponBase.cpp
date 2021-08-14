// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/SWeaponBase.h"

// Engine Includes
#include "Net/UnrealNetwork.h"
#include "AIController.h"

// Game Includes
#include "Components/SMagicChargeComponent.h"
#include "Player/SCharacterBase.h"



// Sets default values
ASWeaponBase::ASWeaponBase()
{
 	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	if (RootComp)
	{
		SetRootComponent(RootComp);
	}

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	if (MeshComp)
	{
		MeshComp->SetupAttachment(GetRootComponent());
	}

	MagicChargeComp = CreateDefaultSubobject<USMagicChargeComponent>(TEXT("MagicChargeComp"));

	bReplicates = true;
}


// Called when the game starts or when spawned
void ASWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	if (MagicChargeComp && MagicChargeComp->CanEverMagicCharge())
	{
		MagicChargeComp->OnMagicChargeChange.AddDynamic(this, &ASWeaponBase::OnMagicChargeStateChange);
	}

}

void ASWeaponBase::SetOwningPawn(ASCharacterBase* NewOwner)
{
	if (NewOwner != MyPawn)
	{
		SetInstigator(NewOwner);
		MyPawn = NewOwner;
		SetOwner(NewOwner);
	}
}

void ASWeaponBase::OnEquip()
{
	AttachMeshToPawn();
}

void ASWeaponBase::OnEnterInventory(ASCharacterBase* NewOwner)
{
	SetOwningPawn(NewOwner);
}

bool ASWeaponBase::GetIsWeaponMagicCharged()
{
	return (MagicChargeComp) ? MagicChargeComp->IsMagicCharged() : false;

}


void ASWeaponBase::OnRep_MyPawn()
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

void ASWeaponBase::OnRep_IsMagicCharged()
{
	bIsMagicCharged ? ApplyMagicChargeEffects() : RemoveMagicChargeEffects();
}


void ASWeaponBase::OnLeaveInventory()
{

}

void ASWeaponBase::AttachMeshToPawn()
{
	if (!MyPawn) return;

	GetRootComponent()->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	MeshComp->SetHiddenInGame(true);

	FName AttackSocketName = MyPawn->GetWeaponSocketName();

	auto OwnerAIController = Cast<AAIController>(MyPawn->GetController());

	// If this weapons new owner is locally controlled then attach to first person mesh
	if (MyPawn->IsLocallyControlled() && !OwnerAIController)
	{
		auto MyPawnFirstPerson = MyPawn->GetFirstPersonMesh();
		if (MyPawnFirstPerson)
		{
			MeshComp->SetHiddenInGame(false);
			GetRootComponent()->AttachToComponent(MyPawnFirstPerson, FAttachmentTransformRules::KeepRelativeTransform, AttackSocketName);
		}
	}
	else // weapon owner is not locally controlled, attach to third person mesh
	{
		auto MyPawnThirdPerson = MyPawn->GetThirdPersonMesh();
		if (MyPawnThirdPerson)
		{
			MeshComp->SetHiddenInGame(false);
			GetRootComponent()->AttachToComponent(MyPawnThirdPerson, FAttachmentTransformRules::KeepRelativeTransform, AttackSocketName);
		}
	}
}


void ASWeaponBase::TriggerOwnerPlayAttackAnimation(const FUseWeaponAnims& Animation)
{
	if (MyPawn)
	{
		UAnimMontage* PerspectiveAnimation = MyPawn->IsFirstPerson() ? Animation.FirstPerson : Animation.ThirdPerson;
		if (PerspectiveAnimation)
		{
			MyPawn->PlayAnimMontage(PerspectiveAnimation);
		}
	}
}


float ASWeaponBase::PlayAnimMontageOnServer(UAnimMontage* Animation)
{
	if (MyPawn && Animation)
	{
		return MyPawn->PlayFirstPersonMontageOnServer(Animation);
	}
	return  0.f; //ANIM_PLAY_FAIL;
}


void ASWeaponBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASWeaponBase, MyPawn);
	DOREPLIFETIME(ASWeaponBase, bIsMagicCharged);
}
	




