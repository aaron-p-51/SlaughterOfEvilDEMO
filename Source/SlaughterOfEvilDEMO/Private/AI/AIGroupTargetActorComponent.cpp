// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AIGroupTargetActorComponent.h"

// Engine Includes
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

// Game Includes
#include "AI/AIGroupController.h"

const static int NO_AI_GROUP_CONTROLLERS = 0;
const static int MULTIPLE_AI_GROUP_CONTROLLERS = 2;
const static int VALID_SENSOR_COUNT = 1;


// Sets default values for this component's properties
UAIGroupTargetActorComponent::UAIGroupTargetActorComponent()
{
}




// Called when the game starts
void UAIGroupTargetActorComponent::BeginPlay()
{
	Super::BeginPlay();

	MyOwner = GetOwner();

	SetupAIGroupTargetComponents();
	
}


void UAIGroupTargetActorComponent::SetupAIGroupTargetComponents()
{
	// Find the worlds AIGroupController
	TArray<AActor*> AIGroupControllers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAIGroupController::StaticClass(), AIGroupControllers);

	if (AIGroupControllers.Num() == NO_AI_GROUP_CONTROLLERS)
	{
		UE_LOG(LogTemp, Error, TEXT("USPlayerAIGroupSensor: The should be exactly one instnace of AAIGroupController in world. None were found!"));
	}
	else if (AIGroupControllers.Num() >= MULTIPLE_AI_GROUP_CONTROLLERS)
	{
		UE_LOG(LogTemp, Error, TEXT("USPlayerAIGroupSensor: The should be exactly one instnace of AAIGroupController in world. Will default for first instance found."));
	}
	else // One AIGroupController found
	{
		WorldAIGroupController = Cast<AAIGroupController>(AIGroupControllers[0]);

		BindAIGroupTargetComponents(EGroupField::EGF_Actor);
		BindAIGroupTargetComponents(EGroupField::EGF_Near);
		BindAIGroupTargetComponents(EGroupField::EGF_Far);
	}

}

void UAIGroupTargetActorComponent::BindAIGroupTargetComponents(EGroupField GroupField)
{
	
	FName SensorFieldTag;
	switch (GroupField)
	{
	case EGroupField::EGF_Actor:
		SensorFieldTag = TEXT("AIGroupTargetActorField");
		break;
	case EGroupField::EGF_Near:
		SensorFieldTag = TEXT("AIGroupTargetNearField");
		break;
	case EGroupField::EGF_Far:
		SensorFieldTag = TEXT("AIGroupTargetFarField");
		break;
	default:
		break;
	}

	TArray<UActorComponent*> OwnerSphereComps = GetOwner()->GetComponentsByTag(USphereComponent::StaticClass(), SensorFieldTag);

	// There should only be one component with tag from above switch
	if (OwnerSphereComps.Num() == VALID_SENSOR_COUNT)
	{
		USphereComponent* AIGroupField = Cast<USphereComponent>(OwnerSphereComps[0]);
		if (AIGroupField && WorldAIGroupController)
		{
			switch (GroupField)
			{
			case EGroupField::EGF_Actor:
				AIGroupField->SetSphereRadius(WorldAIGroupController->GetActorFieldRadius());
				AIGroupField->OnComponentBeginOverlap.AddDynamic(this, &UAIGroupTargetActorComponent::OnAIGroupTargetActorFieldBeginOverlap);
				AIGroupField->OnComponentEndOverlap.AddDynamic(this, &UAIGroupTargetActorComponent::OnAIGroupTargetActorFieldEndOverlap);
				break;
			case EGroupField::EGF_Near:
				AIGroupField->SetSphereRadius(WorldAIGroupController->GetNearFieldRadius());
				AIGroupField->OnComponentBeginOverlap.AddDynamic(this, &UAIGroupTargetActorComponent::OnAIGroupTargetNearFieldBeginOverlap);
				AIGroupField->OnComponentEndOverlap.AddDynamic(this, &UAIGroupTargetActorComponent::OnAIGroupTargetNearFieldEndOverlap);
				break;
			case EGroupField::EGF_Far:
				AIGroupField->SetSphereRadius(WorldAIGroupController->GetFarFieldRadius());
				AIGroupField->OnComponentBeginOverlap.AddDynamic(this, &UAIGroupTargetActorComponent::OnAIGroupTargetFarFieldBeginOverlap);
				AIGroupField->OnComponentEndOverlap.AddDynamic(this, &UAIGroupTargetActorComponent::OnAIGroupTargetFarFieldEndOverlap);
				break;
			default:
				break;
			}
		}
	}
}

void UAIGroupTargetActorComponent::OnAIGroupTargetActorFieldBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && WorldAIGroupController)
	{
		if (OtherActor->ActorHasTag(WorldAIGroupController->AIGroupControlActorTag))
		{
			WorldAIGroupController->OnGroupTargetFieldEnteredEvent(MyOwner, OtherActor, EGroupField::EGF_Actor);
		}
	}
}

void UAIGroupTargetActorComponent::OnAIGroupTargetActorFieldEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && WorldAIGroupController)
	{
		if (OtherActor->ActorHasTag(WorldAIGroupController->AIGroupControlActorTag))
		{
			WorldAIGroupController->OnGroupTargetFieldVacatedEvent(MyOwner, OtherActor, EGroupField::EGF_Actor);
		}
	}
}

void UAIGroupTargetActorComponent::OnAIGroupTargetNearFieldBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && WorldAIGroupController)
	{
		if (OtherActor->ActorHasTag(WorldAIGroupController->AIGroupControlActorTag))
		{
			WorldAIGroupController->OnGroupTargetFieldEnteredEvent(MyOwner, OtherActor, EGroupField::EGF_Near);
		}
	}
}

void UAIGroupTargetActorComponent::OnAIGroupTargetNearFieldEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && WorldAIGroupController)
	{
		if (OtherActor->ActorHasTag(WorldAIGroupController->AIGroupControlActorTag))
		{
			WorldAIGroupController->OnGroupTargetFieldVacatedEvent(MyOwner, OtherActor, EGroupField::EGF_Near);
		}
	}
}

void UAIGroupTargetActorComponent::OnAIGroupTargetFarFieldBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && WorldAIGroupController)
	{
		if (OtherActor->ActorHasTag(WorldAIGroupController->AIGroupControlActorTag))
		{
			WorldAIGroupController->OnGroupTargetFieldEnteredEvent(MyOwner, OtherActor, EGroupField::EGF_Far);
		}
	}
}

void UAIGroupTargetActorComponent::OnAIGroupTargetFarFieldEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && WorldAIGroupController)
	{
		if (OtherActor->ActorHasTag(WorldAIGroupController->AIGroupControlActorTag))
		{
			WorldAIGroupController->OnGroupTargetFieldVacatedEvent(MyOwner, OtherActor, EGroupField::EGF_Far);
		}
	}
}



