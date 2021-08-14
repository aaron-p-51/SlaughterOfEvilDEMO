// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/SCharacterBase.h"
#include "SPlayerBase.generated.h"

class USphereComponent;
class UAIGroupTargetActorComponent;


class USAIGroupCoordinatorTarget;


/**
 * 
 */
UCLASS()
class SLAUGHTEROFEVILDEMO_API ASPlayerBase : public ASCharacterBase
{
	GENERATED_BODY()


public:



protected:

	/*************************************************************************/
	/* AI Group Controller */
	/*************************************************************************/

	UPROPERTY(EditDefaultsOnly)
	USphereComponent* AIGroupControllerTargetActorField;

	UPROPERTY(EditDefaultsOnly)
	USphereComponent* AIGroupControllerTargetNearField;

	UPROPERTY(EditDefaultsOnly)
	USphereComponent* AIGroupControllerTargetFarField;

	UPROPERTY(EditDefaultsOnly)
	UAIGroupTargetActorComponent* AIGroupTargetActorComp;

public:

	ASPlayerBase();

};
