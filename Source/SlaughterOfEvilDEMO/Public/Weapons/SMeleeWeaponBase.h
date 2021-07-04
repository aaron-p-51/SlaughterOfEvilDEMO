// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SMeleeWeaponBase.generated.h"



UCLASS(Abstract, Blueprintable)
class SLAUGHTEROFEVILDEMO_API ASMeleeWeaponBase : public AActor
{
	GENERATED_BODY()
	
/**
 * Members
 */

protected:

	/*************************************************************************/
	/* Components*/
	/*************************************************************************/

	UPROPERTY(VisibleDefaultsOnly)
	USceneComponent* RootComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UStaticMeshComponent* MeshComp;

	/*************************************************************************/
	/* State Variables */
	/*************************************************************************/

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	uint32 bIsMagicCharged : 1;

 /**
  * Methods
  */

public:	
	
	/** Sets default values for this actor's properties */
	ASMeleeWeaponBase();


	/*************************************************************************/
	/* Change Magic State */
	/*************************************************************************/

	UFUNCTION()
	virtual void ApplyMagicCharge() PURE_VIRTUAL(ASMeleeWeaponBase::ApplyMagicCharge, );

	UFUNCTION()
	virtual void RemoveMagicCharge() PURE_VIRTUAL(ASMeleeWeaponBase::RemoveMagicCharge, );


	UFUNCTION()
	virtual bool SetMagicChargeState(bool Charged);


	/*************************************************************************/
	/* Accessors */
	/*************************************************************************/

	UFUNCTION()
	virtual bool IsMagicCharged() const;


};
