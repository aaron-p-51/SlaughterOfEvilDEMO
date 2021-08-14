// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SHealthComponent.h"

// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
	DefaultHealth = 100.f;
}


// Called when the game starts
void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	BindToTakeAnyDamage();
	
}


void USHealthComponent::BindToTakeAnyDamage()
{
	if (GetOwnerRole() == ENetRole::ROLE_Authority)
	{
		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::HandleTakeAnyDamage);
		}
	}
}


void USHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0 || bIsInvincible) return;

	// Do not do more damage than health

	float DamageAmout = (Damage >= Health) ? Health : Damage;

	Health = FMath::Clamp(Health - Damage, 0.f, DefaultHealth);

	OnHealthChange.Broadcast(this, Health, -DamageAmout, DamageType, InstigatedBy, DamageCauser);
}






