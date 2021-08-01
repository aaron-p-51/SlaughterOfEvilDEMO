// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SPlayerAIGroupSensor.h"

// Engine Includes
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

// Game Includes
#include "AI/AIGroupController.h"
#include "Player/SEnemyBase.h"
#include "Player/SPlayerBase.h"


const static int NO_AI_GROUP_CONTROLLERS = 0;
const static int MULTIPLE_AI_GROUP_CONTROLLERS = 2;
const static int VALID_SENSOR_COUNT = 1;

// Sets default values for this component's properties
USPlayerAIGroupSensor::USPlayerAIGroupSensor()
{
}


// Called when the game starts
void USPlayerAIGroupSensor::BeginPlay()
{
	Super::BeginPlay();

	MyOwnerPlayer = Cast<ASPlayerBase>(GetOwner());

	SetupAISensors();
}


void USPlayerAIGroupSensor::SetupAISensors()
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

		SetSensorParameters(EGroupField::EGF_Actor);
		SetSensorParameters(EGroupField::EGF_Near);
		SetSensorParameters(EGroupField::EGF_Far);
	}
}


void USPlayerAIGroupSensor::SetSensorParameters(EGroupField GroupFeild)
{
	FName SensorFieldTag;
	switch (GroupFeild)
	{
		case EGroupField::EGF_Actor:
			SensorFieldTag = TEXT("PlayerAIGroupSensorPlayerField");
			break;
		case EGroupField::EGF_Near:
			SensorFieldTag = TEXT("PlayerAIGroupSensorNearField");
			break;
		case EGroupField::EGF_Far:
			SensorFieldTag = TEXT("PlayerAIGroupSensorFarField");
			break;
		default:
			break;
	}

	TArray<UActorComponent*> OwnerSphereComps = GetOwner()->GetComponentsByTag(USphereComponent::StaticClass(), SensorFieldTag);

	// There should only be one component with tag from above switch
	if (OwnerSphereComps.Num() == VALID_SENSOR_COUNT)
	{
		USphereComponent* AIGroupSensorPlayerField = Cast<USphereComponent>(OwnerSphereComps[0]);
		if (AIGroupSensorPlayerField && WorldAIGroupController)
		{
			switch (GroupFeild)
			{
			case EGroupField::EGF_Actor:
				AIGroupSensorPlayerField->SetSphereRadius(WorldAIGroupController->GetActorFieldRadius());
				AIGroupSensorPlayerField->OnComponentBeginOverlap.AddDynamic(this, &USPlayerAIGroupSensor::OnAIGroupSensorPlayerFieldBeginOverlap);
				AIGroupSensorPlayerField->OnComponentEndOverlap.AddDynamic(this, &USPlayerAIGroupSensor::OnAIGroupSensorPlayerFieldEndOverlap);
				break;
			case EGroupField::EGF_Near:
				AIGroupSensorPlayerField->SetSphereRadius(WorldAIGroupController->GetNearFieldRadius());
				AIGroupSensorPlayerField->OnComponentBeginOverlap.AddDynamic(this, &USPlayerAIGroupSensor::OnAIGroupSensorNearFieldBeginOverlap);
				AIGroupSensorPlayerField->OnComponentEndOverlap.AddDynamic(this, &USPlayerAIGroupSensor::OnAIGroupSensorNearFieldEndOverlap);
				break;
			case EGroupField::EGF_Far:
				AIGroupSensorPlayerField->SetSphereRadius(WorldAIGroupController->GetFarFieldRadius());
				AIGroupSensorPlayerField->OnComponentBeginOverlap.AddDynamic(this, &USPlayerAIGroupSensor::OnAIGroupSensorFarFieldBeginOverlap);
				AIGroupSensorPlayerField->OnComponentEndOverlap.AddDynamic(this, &USPlayerAIGroupSensor::OnAIGroupSensorFarFieldEndOverlap);
				break;
			default:
				break;
			}
		}
	}
	
}


void USPlayerAIGroupSensor::OnAIGroupSensorPlayerFieldBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	/*if (!OtherActor) return;

	ASEnemyBase* Enemy = Cast<ASEnemyBase>(OtherActor);
	if (Enemy && MyOwnerPlayer && WorldAIGroupController)
	{
		WorldAIGroupController->EnemyEnteredPlayerField(MyOwnerPlayer, Enemy);
	}*/
}


void USPlayerAIGroupSensor::OnAIGroupSensorPlayerFieldEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	/*if (!OtherActor) return;

	ASEnemyBase* Enemy = Cast<ASEnemyBase>(OtherActor);
	if (Enemy && MyOwnerPlayer && WorldAIGroupController)
	{
		WorldAIGroupController->EnemyLeftPlayerField(MyOwnerPlayer, Enemy);
	}*/
}

void USPlayerAIGroupSensor::OnAIGroupSensorNearFieldBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	/*if (!OtherActor) return;

	ASEnemyBase* Enemy = Cast<ASEnemyBase>(OtherActor);
	if (Enemy && MyOwnerPlayer && WorldAIGroupController)
	{
		WorldAIGroupController->EnemyEnteredNearField(MyOwnerPlayer, Enemy);
	}*/
}

void USPlayerAIGroupSensor::OnAIGroupSensorNearFieldEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	/*if (!OtherActor) return;

	ASEnemyBase* Enemy = Cast<ASEnemyBase>(OtherActor);
	if (Enemy && MyOwnerPlayer && WorldAIGroupController)
	{
		WorldAIGroupController->EnemyLeftNearField(MyOwnerPlayer, Enemy);
	}*/
}

void USPlayerAIGroupSensor::OnAIGroupSensorFarFieldBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	/*if (!OtherActor) return;

	ASEnemyBase* Enemy = Cast<ASEnemyBase>(OtherActor);
	if (Enemy && MyOwnerPlayer && WorldAIGroupController)
	{
		WorldAIGroupController->EnemyEnteredFarField(MyOwnerPlayer, Enemy);
	}*/
}

void USPlayerAIGroupSensor::OnAIGroupSensorFarFieldEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	/*if (!OtherActor) return;

	ASEnemyBase* Enemy = Cast<ASEnemyBase>(OtherActor);
	if (Enemy && MyOwnerPlayer && WorldAIGroupController)
	{
		WorldAIGroupController->EnemyLeftFarField(MyOwnerPlayer, Enemy);
	}*/
}



