// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/SLongsword.h"

// Engine Includes
#include "Materials/MaterialInstanceDynamic.h"

// Game Includes
#include "Components/SMagicChargeComponent.h"

ASLongsword::ASLongsword()
{

	PrimaryActorTick.bCanEverTick = true;

	// All Collisions will handled
	if (MeshComp)
	{
		MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	MagicChargeComp = CreateDefaultSubobject<USMagicChargeComponent>(TEXT("MagicChargeComp"));
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



//void ASLongsword::OnRep_SetMagicCharge()
//{
//	Super::OnRep_SetMagicCharge();
//
//	if (MeshDynamicMaterial)
//	{
//		if (bIsMagicCharged)
//		{
//			MeshDynamicMaterial->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor(1.f, 0.f, 0.f, 0.f));
//		}
//		else
//		{
//			MeshDynamicMaterial->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor(0.f, 1.f, 0.f, 0.f));
//		}
//	}
//}

//bool ASLongsword::TrySetMagicCharge(bool Charged)
//{
//	Super::TrySetMagicCharge(Charged);
//
//	if (bIsMagicCharged && ) 
//
//	if (SetMagicChargeState(true))
//	{
//		if (MeshDynamicMaterial && IsMagicCharged())
//		{
//			MeshDynamicMaterial->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor(1.f, 0.f, 0.f, 0.f));
//		}
//	}
//}


//void ASLongsword::RemoveMagicCharge()
//{
//	if (SetMagicChargeState(false))
//	{
//		if (MeshDynamicMaterial && !IsMagicCharged())
//		{
//			MeshDynamicMaterial->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor(0.f, 1.f, 0.f, 0.f));
//		}
//	}
//}

