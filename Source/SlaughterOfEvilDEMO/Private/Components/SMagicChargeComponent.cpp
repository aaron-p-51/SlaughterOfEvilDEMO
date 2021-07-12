// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SMagicChargeComponent.h"

// Sets default values for this component's properties
USMagicChargeComponent::USMagicChargeComponent()
{


}




// Called when the game starts
void USMagicChargeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


bool USMagicChargeComponent::TrySetMagicCharge(bool Charged)
{
	if (GetOwnerRole() == ENetRole::ROLE_Authority)
	{
		if (Charged && !bIsMagicCharged)
		{
			bIsMagicCharged = true;
			UE_LOG(LogTemp, Warning, TEXT("Set bIsMagicCharged"));
			return true;
		}
		else if (!Charged && bIsMagicCharged)
		{
			bIsMagicCharged = false;
			return true;
		}
	}

	return false;
}




