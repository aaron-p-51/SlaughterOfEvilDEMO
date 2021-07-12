// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SMagicChargeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLAUGHTEROFEVILDEMO_API USMagicChargeComponent : public UActorComponent
{
	GENERATED_BODY()

/**
 * Members
 */
 private:

	uint32 bIsMagicCharged : 1;

 /**
  * Methods
  */

public:	
	// Sets default values for this component's properties
	USMagicChargeComponent();

	bool TrySetMagicCharge(bool Charged);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


		
};
