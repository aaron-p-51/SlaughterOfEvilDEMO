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

	/** Is the attached actor currently magic charged, will only be valid on server */
	UPROPERTY(VisibleAnywhere)
	uint32 bIsMagicCharged : 1;

	/** Does this components actors owner have to face magic charge source for successful magic charge */
	UPROPERTY(EditDefaultsOnly)
	uint32 bOwnerMustFaceChargeSource : 1;

	/** Max angle in degrees this components actors owner can face to for sucessfull magic charge  */
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bOwnerMustFaceChargeSource"), meta = (ClampMin = "0", ClampMax = "180", UIMin = "0", UIMax = "180"))
	float MaxOwnerFaceAwayFromChargeSource;


 /**
  * Methods
  */

public:	
	// Sets default values for this component's properties
	USMagicChargeComponent();

	bool TrySetMagicCharge(bool Charged);

	FORCEINLINE bool IsMagicCharged() const { return bIsMagicCharged; }

	FORCEINLINE bool OwnerMustFaceChargeSource() const { return bOwnerMustFaceChargeSource; }

	FORCEINLINE float GetMaxOwnerFaceAwayFromChargeSource() const { return MaxOwnerFaceAwayFromChargeSource; }

	/**
	 * [Server] 
	 */
	UPROPERTY()
	FOnMagicChargeChangeSignature OnMagicChargeChange;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


		
};
