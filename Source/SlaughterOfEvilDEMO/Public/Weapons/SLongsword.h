// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/SMeleeWeaponBase.h"
#include "SLongsword.generated.h"

class UMaterialInstanceDynamic;
class USMagicChargeComponent;


/**
 * 
 */
UCLASS()
class SLAUGHTEROFEVILDEMO_API ASLongsword : public ASMeleeWeaponBase
{
	GENERATED_BODY()
	

/**
 * Members
 */

protected:

	/** Material will change color to indicate magic charge state */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UMaterialInstanceDynamic* MeshDynamicMaterial;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USMagicChargeComponent* MagicChargeComp;

/**
 * Methods
 */

 public:

	 ASLongsword();

	 // Called every frame
	 virtual void Tick(float DeltaTime) override;

	/*************************************************************************/
	/* Change Magic State */
	/*************************************************************************/

	/*virtual void ApplyMagicCharge() override;

	virtual void RemoveMagicCharge() override;*/


	/* virtual void OnRep_SetMagicCharge() override;*/


protected:

	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;
};
