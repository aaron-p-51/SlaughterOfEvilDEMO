// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/AIGroupController.h"
#include "Components/ActorComponent.h"
#include "AIGroupControlComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTriggerRangeAttackSignature, AActor*, RangeAttackTarget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTriggerAttackSignature, AActor*, AttackTarget);

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

	UPROPERTY()
	FOnTriggerRangeAttackSignature OnTriggerRangeAttack;
	
	UPROPERTY()
	FOnTriggerAttackSignature OnTriggerAttack;

public:	
	// Sets default values for this component's properties
	UAIGroupControlComponent();


	void TriggerRangeAttack() const;

	void TriggerAttack()const;


		
};
