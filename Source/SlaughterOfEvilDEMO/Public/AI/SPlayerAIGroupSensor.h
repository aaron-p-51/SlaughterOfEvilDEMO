// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIGroupController.h"
#include "Components/ActorComponent.h"
#include "SPlayerAIGroupSensor.generated.h"

class USphereComponent;
class AAIGroupController;
class ASPlayerBase;



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLAUGHTEROFEVILDEMO_API USPlayerAIGroupSensor : public UActorComponent
{
	GENERATED_BODY()

/**
 * Members
 */

protected:
	
	ASPlayerBase* MyOwnerPlayer;

	AAIGroupController*	WorldAIGroupController;


public:	
	// Sets default values for this component's properties
	USPlayerAIGroupSensor();



protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnAIGroupSensorPlayerFieldBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnAIGroupSensorPlayerFieldEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnAIGroupSensorNearFieldBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnAIGroupSensorNearFieldEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnAIGroupSensorFarFieldBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnAIGroupSensorFarFieldEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


private:

	void SetupAISensors();
	void SetSensorParameters(EGroupField GroupFeild);




		
};
