// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AIGroupControlComponent.h"

// Sets default values for this component's properties
UAIGroupControlComponent::UAIGroupControlComponent()
{

}

void UAIGroupControlComponent::TriggerRangeAttack() const
{
	OnTriggerRangeAttack.Broadcast(GroupControllerData.TargetActor);
}

void UAIGroupControlComponent::TriggerAttack() const
{
	OnTriggerAttack.Broadcast(GroupControllerData.TargetActor);
}


