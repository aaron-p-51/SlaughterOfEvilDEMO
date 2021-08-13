// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SMagicChargeComponent.generated.h"


// OnMagicChargeChanged event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMagicChargeChangeSignature, bool, IsMagicCharged);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLAUGHTEROFEVILDEMO_API USMagicChargeComponent : public UActorComponent
{
	GENERATED_BODY()

/**
 * Members
 */
 private:

	UPROPERTY(EditDefaultsOnly)
	uint32 bCanEverMagicCharge : 1;

	/** Can Charge be applied */
	UPROPERTY(EditDefaultsOnly)
	uint32 bCanMagicCharge : 1;

	/** Is the attached actor currently magic charged, will only be valid on server */
	UPROPERTY(VisibleAnywhere)
	uint32 bIsMagicCharged : 1;

	/** Does this components actors owner have to face magic charge source for successful magic charge */
	UPROPERTY(EditDefaultsOnly)
	uint32 bOwnerMustFaceChargeSource : 1;

	/** Max angle in degrees this components actors owner can face to for sucessfull magic charge  */
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bOwnerMustFaceChargeSource"), meta = (ClampMin = "0", ClampMax = "180", UIMin = "0", UIMax = "180"))
	float MaxOwnerFaceAwayAngleFromChargeSource;


 /**
  * Methods
  */

public:	
	// Sets default values for this component's properties
	USMagicChargeComponent();

	FORCEINLINE bool CanEverMagicCharge() const { return bCanEverMagicCharge; }

	/**
	 * [Server] Try and set the magic charge state. If state is changed OnMagicChargeChange will brodcast change
	 * 
	 * return was bIsMagicCharged changed
	 */
	bool TrySetMagicCharge(const AActor* ActorApplyingCharge);

	bool TryRemoveMagicCharge();

	bool OwnerWithinValidAngle(const AActor* ActorApplyingCharge);

	/**
	 * [Server] Is the owning actor magic charged
	 */
	FORCEINLINE bool IsMagicCharged() const { return bIsMagicCharged; }

	void SetCanMagicCharge(bool value) { bCanMagicCharge = value; }

	///**
	// * [Server] Does the owning actor of this components owner (example, SMagicComponent->SMeleeWeaponBase->CharacterBase) need
	// * to face a certain direction in relation to the actor trying to apply the magic charge
	// */
	//FORCEINLINE bool OwnerMustFaceChargeSource() const { return bOwnerMustFaceChargeSource; }

	///**
	// * [Server] Get the max angle to owning actor of this components owner (example, SMagicComponent->SMeleeWeaponBase->CharacterBase) can face
	// * away from the forward vector of the actor trying to apply the magic charge
	// */
	//FORCEINLINE float GetMaxOwnerFaceAwayFromChargeSource() const { return MaxOwnerFaceAwayAngleFromChargeSource; }

	/**
	 * [Server] Brodcast event when bIsMagicCharged changes
	 */
	UPROPERTY()
	FOnMagicChargeChangeSignature OnMagicChargeChange;
		
};
