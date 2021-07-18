// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SMagicChargeComponent.h"

// Sets default values for this component's properties
USMagicChargeComponent::USMagicChargeComponent()
{
	bIsMagicCharged = false;
}


bool USMagicChargeComponent::TrySetMagicCharge(const AActor* ActorApplyingCharge)
{
	// Enusre running on server, valid  input parameter and not currently charged
	if (GetOwnerRole() == ENetRole::ROLE_Authority && ActorApplyingCharge && !bIsMagicCharged)
	{
		// If owner must face charging source and not within valid angle then charge will not be applies
		if ((bOwnerMustFaceChargeSource) && (!OwnerWithinValidAngle(ActorApplyingCharge))) return false;

		bIsMagicCharged = true;
		OnMagicChargeChange.Broadcast(bIsMagicCharged);
		return bIsMagicCharged;
	}

	return false;
}


bool USMagicChargeComponent::OwnerWithinValidAngle(const AActor* ActorApplyingCharge)
{
	if (!ActorApplyingCharge) return false;

	auto MyOwnersOwner = GetOwner()->GetOwner();
	if (MyOwnersOwner)
	{
		// Get the dot product between owner of the owner of this MagicChargeComponent and the actor applying the magic charge
		float Dot = FVector::DotProduct(ActorApplyingCharge->GetActorForwardVector(), MyOwnersOwner->GetActorForwardVector());

		// Map dot product range to valid range of MaxOwnerFaceAwayFromChargeSource (0 to 180)
		float MapDot = FMath::GetMappedRangeValueUnclamped(FVector2D(-1.f, 1.f), FVector2D(0.f, 180.f), Dot);

		// If the mapped dot product value is less than or equal to the max angle HitActorOwner can face from the projectile
		// then HitActorMagicChargeComp can accept magic charge
		return MapDot <= FMath::Abs(MaxOwnerFaceAwayAngleFromChargeSource);
	}

	return false;
}

bool USMagicChargeComponent::TryRemoveMagicCharge()
{
	if (GetOwnerRole() != ENetRole::ROLE_Authority) return false;

	if (bIsMagicCharged)
	{
		bIsMagicCharged = false;
		OnMagicChargeChange.Broadcast(bIsMagicCharged);
		return true;
	}
	
	return false;
}




