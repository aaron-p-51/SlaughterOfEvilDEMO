// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/AIGroupController.h"
#include "AIController.h"
#include "SAIControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class SLAUGHTEROFEVILDEMO_API ASAIControllerBase : public AAIController
{
	GENERATED_BODY()
	
public:

	

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FAIGroupControlData GroupControllerSetData;

};
