// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/SLongsword.h"

// Engine Includes
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/GameplayStatics.h"

// Game Includes
#include "Magic/SMagicProjectileBase.h"

ASLongsword::ASLongsword()
{

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

void ASLongsword::ReleaseMagicCharge(FTransform ReleaseTransform)
{
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		if (DefaultMagicProjectile)
		{
			auto MagicProjectile = Cast<ASMagicProjectileBase>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, DefaultMagicProjectile, ReleaseTransform));
			if (MagicProjectile)
			{
				MagicProjectile->SetOwner(this);
				MagicProjectile->SetInstigator(Cast<APawn>(GetOwner()));
				UGameplayStatics::FinishSpawningActor(MagicProjectile, ReleaseTransform);
			}
		}
	}
}


