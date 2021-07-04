// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/SMeleeWeaponBase.h"
#include "SLongsword.generated.h"

class UMaterialInstanceDynamic;


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

	/*************************************************************************/
	/* Change Magic State */
	/*************************************************************************/

	virtual void ApplyMagicCharge() override;

	virtual void RemoveMagicCharge() override;


protected:

	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;
};
