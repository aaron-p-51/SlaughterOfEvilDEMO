// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIGroupController.h"
#include "Components/ActorComponent.h"
#include "AIGroupTargetActorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLAUGHTEROFEVILDEMO_API UAIGroupTargetActorComponent : public UActorComponent
{
	GENERATED_BODY()



/**
 * Members
 */
private:

	/** Owing actor of this component */
	AActor* MyOwner;

	/** AI group controller in world, should only every be one instance of AAIGroupController in world*/
	AAIGroupController* WorldAIGroupController;


/**
 * Members
 */
public:	
	// Sets default values for this component's properties
	UAIGroupTargetActorComponent();

	/**
	 * Bound to component owner's AI group target ActorField. The ActorField is the field that immediately surrounds the target actor. Will notify AIGroupController when an AI group controlled actor enters the
	 * actor field of a AIGroupController's TargetActor.
	 */
	UFUNCTION()
	void OnAIGroupTargetActorFieldBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/**
	 * Bound to component owner's AI group target ActorField. The ActorField is the field that immediately surrounds the target actor. Will notify AIGroupController when an AI group controlled actor leaves the
	 * actor field of a AIGroupController's TargetActor.
	 */
	UFUNCTION()
	void OnAIGroupTargetActorFieldEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/**
	 * Bound to component owner's AI group target NearField. The NearField is the field that surrounds the ActorField of the target actor. Will notify AIGroupController when an AI group controlled actor enters the
	 * NearField of a AIGroupController's TargetActor.
	 */
	UFUNCTION()
	void OnAIGroupTargetNearFieldBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/**
	 * Bound to component owner's AI group target NearField. The NearField is the field that surrounds the ActorField of the target actor. Will notify AIGroupController when an AI group controlled actor leaves the
	 * NearField of a AIGroupController's TargetActor.
	 */
	UFUNCTION()
	void OnAIGroupTargetNearFieldEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/**
	* Bound to component owner's AI group target FarField. The FarField is the field that surrounds the NearField of the target actor. Will notify AIGroupController when an AI group controlled actor enters the
	* FarField of a AIGroupController's TargetActor.
	*/
	UFUNCTION()
	void OnAIGroupTargetFarFieldBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/**
	* Bound to component owner's AI group target FarField. The FarField is the field that surrounds the NearField of the target actor. Will notify AIGroupController when an AI group controlled actor leaves the
	* FarField of a AIGroupController's TargetActor.
	*/
	UFUNCTION()
	void OnAIGroupTargetFarFieldEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:

	/** Helper method to set and register Components overlap callbacks with AIGroupController */
	void SetupAIGroupTargetComponents();

	/**
	 * Bind Components with AIGroupController
	 */
	void BindAIGroupTargetComponents(EGroupField GroupField);


		
};
