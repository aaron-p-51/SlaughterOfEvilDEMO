// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SPlayerBase.h"

// Engine Includes
#include "Components/SphereComponent.h"


// Game Includes
#include "AI/AIGroupTargetActorComponent.h"




ASPlayerBase::ASPlayerBase()
{
	
	AIGroupControllerTargetActorField = CreateDefaultSubobject<USphereComponent>(TEXT("AIGroupControllerTargetActorField"));
	if (AIGroupControllerTargetActorField)
	{
		AIGroupControllerTargetActorField->SetupAttachment(GetRootComponent());
		AIGroupControllerTargetActorField->SetGenerateOverlapEvents(true);
		AIGroupControllerTargetActorField->SetCollisionProfileName(TEXT("AIGroupField"));
		AIGroupControllerTargetActorField->ComponentTags.Add(FName("AIGroupTargetActorField"));
	}

	AIGroupControllerTargetNearField = CreateDefaultSubobject<USphereComponent>(TEXT("AIGroupControllerTargetNearField"));
	if (AIGroupControllerTargetNearField)
	{
		AIGroupControllerTargetNearField->SetupAttachment(GetRootComponent());
		AIGroupControllerTargetNearField->SetGenerateOverlapEvents(true);
		AIGroupControllerTargetNearField->SetCollisionProfileName(TEXT("AIGroupField"));
		AIGroupControllerTargetNearField->ComponentTags.Add(FName("AIGroupTargetNearField"));
	}

	AIGroupControllerTargetFarField = CreateDefaultSubobject<USphereComponent>(TEXT("AIGroupControllerTargetFarField"));
	if (AIGroupControllerTargetFarField)
	{
		AIGroupControllerTargetFarField->SetupAttachment(GetRootComponent());
		AIGroupControllerTargetFarField->SetGenerateOverlapEvents(true);
		AIGroupControllerTargetFarField->SetCollisionProfileName(TEXT("AIGroupField"));
		AIGroupControllerTargetFarField->ComponentTags.Add(FName("AIGroupTargetFarField"));
	}

	AIGroupTargetActorComp = CreateDefaultSubobject<UAIGroupTargetActorComponent>(TEXT("AIGroupTargetActorComp"));


}

