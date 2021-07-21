// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/SCharacterBase.h"
#include "SEnemyBase.generated.h"

/**
 * 
 */
UCLASS()
class SLAUGHTEROFEVILDEMO_API ASEnemyBase : public ASCharacterBase
{
	GENERATED_BODY()

/**
 * Members
 */

protected:

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* AttackForwardMontage;


 /**
  * Methods
  */

public:
	virtual void MeleeAttack() override;
	
};
