// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/AIGroupController.h"
#include "Components/ActorComponent.h"
#include "SAIGroupControlledActor.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLAUGHTEROFEVILDEMO_API USAIGroupControlledActor : public UActorComponent
{
	GENERATED_BODY()


/**
 * Members
 */

 public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FAIGroupControlData GroupControllerData;

/**
 * Methods
 */
public:	
	// Sets default values for this component's properties
	USAIGroupControlledActor();
		
};
