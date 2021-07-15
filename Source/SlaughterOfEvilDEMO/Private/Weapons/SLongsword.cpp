// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/SLongsword.h"

// Engine Includes
#include "Materials/MaterialInstanceDynamic.h"

// Game Includes

#include "SMeleeWeaponWielder.h"

ASLongsword::ASLongsword()
{

	// All Collisions will handled
	if (MeshComp)
	{
		MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	
}




void ASLongsword::BeginPlay()
{
	Super::BeginPlay();

	if (MeshComp)
	{
		MeshDynamicMaterial = MeshComp->CreateAndSetMaterialInstanceDynamic(0);
	}

	
}


void ASLongsword::ApplyMagicChargeEffects()
{
	if (MeshDynamicMaterial)
	{
		MeshDynamicMaterial->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor(1.f, 0.f, 0.f, 0.f));
	}
}

void ASLongsword::RemoveMagicChargeEffects()
{
	if (MeshDynamicMaterial)
	{
		MeshDynamicMaterial->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor(0.f, 1.f, 0.f, 0.f));
	}
}




