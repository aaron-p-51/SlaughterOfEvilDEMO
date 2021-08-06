// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/SCharacterBase.h"
#include "SEnemyBase.generated.h"


class UAIGroupControlComponent;

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


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UAIGroupControlComponent* AIGroupControlComp;


 /**
  * Methods
  */

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void AIGroupControllerTriggerMeleeAttack(AActor* GroupTarget);

	UFUNCTION()
	void AIGroupControllerTriggerRangeAttack(AActor* GroupTarget);

public:

	ASEnemyBase();

	virtual void MeleeAttack() override;

	int32 FindMeleeAttackInRangeOfGroupTarget();

	virtual void ProjectileMagicAttack() override;

	virtual void SpawnMagicAttackProjectile() override;

	int32 FindValidProjectileMagicAttackIndex();

	bool IsProjectileViewUnobstructed(FVector& ProjectileLaunchPosition, FVector& ProjectileTargetPosition, float Radius);

	
	
};
