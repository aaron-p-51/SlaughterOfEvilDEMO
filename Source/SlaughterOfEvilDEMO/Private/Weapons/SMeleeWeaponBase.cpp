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

bool ASMeleeWeaponBase::IsMagicCharged() const
{
	return bIsMagicCharged;
}



