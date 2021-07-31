// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/AIGroupController.h"
#include "Components/ActorComponent.h"
#include "AIGroupControlComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLAUGHTEROFEVILDEMO_API UAIGroupControlComponent : public UActorComponent
{
	GENERATED_BODY()

/**
 * Members
 */

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FAIGroupControlData GroupControllerData;

public:	
	// Sets default values for this component's properties
	UAIGroupControlComponent();



		
};
