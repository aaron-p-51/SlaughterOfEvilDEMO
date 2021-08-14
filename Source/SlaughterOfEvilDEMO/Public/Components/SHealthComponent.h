// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SHealthComponent.generated.h"

// OnHealthChanged event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangeSignature, USHealthComponent*, HealthComp, float, Health, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, HealthChangeCauser);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLAUGHTEROFEVILDEMO_API USHealthComponent : public UActorComponent
{
	GENERATED_BODY()


/**
 * Members
 */
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthComponent")
	bool bIsInvincible;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HealthComponent")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HealthComponent")
	float DefaultHealth;


 /**
  * Methods
  */
public:	
	// Sets default values for this component's properties
	USHealthComponent();

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChangeSignature OnHealthChange;

	void AddHealth(float Amount);

	FORCEINLINE float GetHealth() const { return Health; }




protected:
	// Called when the game starts
	virtual void BeginPlay() override;


	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

private:

	void BindToTakeAnyDamage();


		
};
