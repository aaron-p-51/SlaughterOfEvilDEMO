// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/SMeleeWeaponBase.h"

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


