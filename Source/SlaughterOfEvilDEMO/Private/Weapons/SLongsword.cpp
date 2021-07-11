// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/SLongsword.h"

// Engine Includes
#include "Materials/MaterialInstanceDynamic.h"

// Game Includes

ASLongsword::ASLongsword()
{

	PrimaryActorTick.bCanEverTick = true;

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
	};
}

void ASLongsword::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}



void ASLongsword::ApplyMagicCharge()
{
	if (SetMagicChargeState(true))
	{
		if (MeshDynamicMaterial && IsMagicCharged())
		{
			MeshDynamicMaterial->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor(1.f, 0.f, 0.f, 0.f));
		}
	}
}


void ASLongsword::RemoveMagicCharge()
{
	if (SetMagicChargeState(false))
	{
		if (MeshDynamicMaterial && !IsMagicCharged())
		{
			MeshDynamicMaterial->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor(0.f, 1.f, 0.f, 0.f));
		}
	}
}

