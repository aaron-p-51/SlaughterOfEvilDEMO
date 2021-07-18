// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/SMeleeWeaponBase.h"
#include "SLongsword.generated.h"

class UMaterialInstanceDynamic;
class USMagicChargeComponent;
class ASMagicProjectileBase;

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


/**
 * Methods
 */

 public:

	 ASLongsword();



	/*************************************************************************/
	/* Change Magic State */
	/*************************************************************************/

	virtual void ApplyMagicChargeEffects() override;

	virtual void RemoveMagicChargeEffects() override;

	virtual void ReleaseMagicCharge(FTransform& ReleaseTransform) override;


protected:

	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;




};
