// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AIGroupController.generated.h"


class ASEnemyBase;
class ASPlayerBase;
class UAIGroupControlComponent;






/**
 * Struct for each Actor, usually a player's characters, that has actors targeting it via this
 * AIGroupController. Each actor that has a AIGroupTargetActorComponents will have a corresponding
 * FAIControlledGroup struct assigned to it. Each one of these actors with a AIGroupTargetActorComponent
 * are referred to as a GroupTarget
 */
USTRUCT(BlueprintType)
struct FAIControlledGroup
{
	GENERATED_BODY()

	/** All of the actors currently controlled by instance of AIGroupController that are
	 *  targeting a GroupTarget  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TSet<AActor*> GroupActors;

	/** Current numbers of actors in GroupTargets near field  */
	UPROPERTY()
	int32 CurrentNearFieldActorCount = 0;

	/** Current number of actors in GroupTargets far field */
	UPROPERTY()
	int32 CurrentFarFieldActorCount = 0;

};

/**
 * Fields surrounding the GroupTarget.
 */
UENUM(BlueprintType)
enum class EGroupField : uint8
{
	EGF_None				UMETA(DisplayName = "None"),	// outside of field recognized by AIGroupController 
	EGF_Far					UMETA(DisplayName = "Far"),		// Outermost field
	EGF_Near				UMETA(DisplayName = "Near"),	// Field outside EGF_Actor and inside EFG_Far 
	EGF_Actor				UMETA(DisplayName = "Actor")	// Field that immediately surrounds GroupTarget Actor
};


UENUM(BlueprintType)
enum class EFOVRange : uint8
{
	EFR_MaxFOV				UMETA(DisplayName = "MaxFOV"),
	EFR_AttackFOV			UMETA(DisplayName = "AttackFOV")
};

UENUM(BlueprintType)
enum class EValidFieldResult : uint8
{
	EVFR_InValidRange		UMETA(DisplayName = "InValidRange"),
	EVFR_LessThanMin		UMETA(DisplayName = "LessThanMin"),
	EVFR_GreaterThanMax		UMETA(DisplayName = "GreaterThanMax")
};


/**
 * struct for each actor that is part of a AI Control Group, ie Every actor that has a AIGroupControlComponent 
 * each one of these actors is referred to as an AI Group Controlled Actor. Multiple AI Group Controlled Actors are assigned to a
 * Group Target.
 */
USTRUCT(BlueprintType)
struct FAIGroupControlData
{
	GENERATED_BODY()

	/** Current assigned group field */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	EGroupField AssignedGroupField = EGroupField::EGF_None;

	/** Current field the AIGroupControlledActor is location in */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	EGroupField CurrentGroupField = EGroupField::EGF_None;

	/** Location the AIGroupControlledActor is directed to move/stay at */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FVector DesiredLocation;

	/** GroupTarget for AIGroupControlledActor */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	AActor* TargetActor;

	UPROPERTY()
	int32 AttackCounter;

	UPROPERTY()
	int32 RangeAttackCounter;

};



UCLASS()
class SLAUGHTEROFEVILDEMO_API AAIGroupController : public AActor
{
	GENERATED_BODY()
	
/**
 * Members
 */

private:

	/** Cached variable, half of NearFieldAttackFOV  */
	float HalfNearFieldAttackFOV;
	/** Cached variable, half of NearFieldMaxFOV  */
	float HalfNearFieldMaxFOV;
	/** Cached variable, half of FarFieldAttackFOV  */
	float HalfFarFieldAttackFOV;
	/** Cached variable, half of FarFieldMaxFOV  */
	float HalfFarFieldMaxFOV;

protected:

	/*************************************************************************/
	/* Near Field */
	/*************************************************************************/

	/** Max number of AI Group Controlled Actors allowed in a Group Targets Near Field  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "nearField")
	int32 NearFieldMaxActorCount;

	/** Near field radius surrounding the Group Target (Actor Field Radius < Near Field Radius < Far Field Radius)  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NearField")
	float NearFieldRadius;

	/** Attack FOV in near field, max angle an AI Group Controlled Actor can be from Group Targets forward vector to attack */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NearField", meta = (ClampMin = "0", ClampMax = "360", UIMin = "0", UIMax = "360"))
	float NearFieldAttackFOV;

	/** Max FOV in near field, max angle an AI Group Controlled Actor can be from Group Targets forward vector. If outside this angle
	 *  AI Group Target will be assigned a new destination
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NearField", meta = (ClampMin = "0", ClampMax = "360", UIMin = "0", UIMax = "360"))
	float NearFieldMaxFOV;

	/** Draw near field area, will draw current radius, max, and attack for near field */
	UPROPERTY(EditInstanceOnly, Category = "NearField")
	bool DrawNearFieldDebug;

	/** Color for DrawNearFieldDebug */
	UPROPERTY(EditInstanceOnly, Category = "NearField")
	FColor NearFieldDebugColor;



	/*************************************************************************/
	/* Far Field */
	/*************************************************************************/

	/** Max number of AI Group Controlled Actors allowed in a Group Targets Far Field  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "nearField")
	int32 FarFieldMaxActorCount;

	/** Far field radius surrounding the Group Target (Actor Field Radius < Near Field Radius < Far Field Radius)  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FarField")
	float FarFieldRadius;

	/** Attack FOV in far field, max angle an AI Group Controlled Actor can be from Group Targets forward vector to attack */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FarField", meta = (ClampMin = "0", ClampMax = "360", UIMin = "0", UIMax = "360"))
	float FarFieldAttackFOV;

	/** Max FOV in far field, max angle an AI Group Controlled Actor can be from Group Targets forward vector. If outside this angle
	 *  AI Group Target will be assigned a new destination
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FarField", meta = (ClampMin = "0", ClampMax = "360", UIMin = "0", UIMax = "360"))
	float FarFieldMaxFOV;

	/** Draw far Field area, will draw current radius, max, and attack for far field */
	UPROPERTY(EditInstanceOnly, Category = "FarField")
	bool DrawFarFieldDebug;

	/** Color for DrawFarFieldDebug */
	UPROPERTY(EditInstanceOnly, Category = "FarField")
	FColor FarFieldDebugColor;



	/*************************************************************************/
	/* Actor Field */
	/*************************************************************************/

	/** Actor field radius surrounding the Group Target (Actor Field Radius < Near Field Radius < Far Field Radius)  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ActorField")
	float ActorFieldRadius;

	/** Draw actor Field area, will draw current radius for actor field */
	UPROPERTY(EditInstanceOnly, Category = "ActorField")
	bool DrawActorFieldDebug;

	/** Color for DrawActorFieldDebug */
	UPROPERTY(EditInstanceOnly, Category = "ActorField")
	FColor ActorFieldDebugColor;

	/*************************************************************************/
	/* AI Controlled Groups */
	/*************************************************************************/

	/**
	 * Groups for each Group Target. Each Group Target is mapped to a FAIControlledGroup. The AIControlledGroup struct contains
	 * all of the AI Controlled Group Actors that are assigned a specific Group Target
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TMap<AActor*, FAIControlledGroup> AIControlledGroups;

	/** Timer period for OnGroupTimerTick should be as low as possible when called every Actor in every AIControlledGroup will update */
	UPROPERTY(EditAnywhere)
	float GroupTimerPeriod;

public:

	/** Used to identify actors that can be controlled by this AIGroupController */
	const FName AIGroupControlActorTag = TEXT("AIGroupControlledActor");

 /**
  * Methods
  */


private:	

	/** Timer handle for calling OnGroupTimerTick */
	FTimerHandle GroupTickTimerHandle;

	/**
	 * Draw debug marks
	 * 
	 * @param GroupTarget	Target do draw debug lines. Actor must be in AIControlledGroups
	 */
	void DrawDebug(AActor* GroupTarget);

	/**
	 * Simple helper function go get point on unit circle  
	 */
	FVector PointOnCircle(float Angle, float DistanceFromCenter) const;

	/**
	 * 
	 */
	float GetAngleGroupTargetRightVectorToTargetLocation(AActor* GroupTarget, FVector TargetLocation) const;

	const FString EnumToString(const TCHAR* Enum, int32 EnumValue) const;

	FVector GetDesiredLocationMaxSeparation(AActor* GroupTarget, AActor* AIGroupControlledActor) const;

	UAIGroupControlComponent* GetAIGroupControlComponent(AActor* AIGroupControlledActor) const;



	void AssignGroupField(AActor* AIGroupControlledActor, EGroupField GroupField) const;

	EGroupField IncreaseFieldPresence(AActor* AIGroupControlledActor);

	EGroupField DecreaseFieldPresence(AActor* AIGroupControlledActor);

	void GetAllAIGroupControlActorsInField(AActor* GroupTarget, EGroupField GroupField, TArray<AActor*>& ActorsInField);





protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void OnGroupTimerTick();

	UFUNCTION(BlueprintImplementableEvent, Category = "AIGroupController | Events")
	void BP_OnGroupTimerTick();

	/**
	 * Get current number of AI Group Controlled Actors that are assigned to GroupTarget and are assigned
	 * GroupField
	 * 
	 * @param GroupTarget		Target for AI Group Controlled Actors
	 * @param GroupField		GroupField to check currently assigned
	 */
	int32 GetNumberOfAIGroupActorsAssignedToField(AActor* GroupTarget, EGroupField GroupField);

	AActor* GetFarthestGroupControlledActorInField(AActor* GroupTarget, EGroupField Field);

	AActor* GetClosestGroupControlledActorInField(AActor* GroupTarget, EGroupField Field);

	UFUNCTION(BlueprintCallable, Category = "AIGroupController")
	bool IsAllowedInNearFeild(AActor* AIGroupControlledActor) const;

	UFUNCTION(BlueprintCallable, Category = "AIGroupController")
	EValidFieldResult InValidFieldRange(AActor* GroupTarget, AActor* AIGroupControlledActor) const;

	UFUNCTION(BlueprintCallable, Category = "AIGroupController")
	bool InValidFOV(AActor* GroupTarget, AActor* AIGroupControlledActor, EFOVRange FOVRange) const;

	UFUNCTION(BlueprintCallable, Category = "AIGroupController")
	bool IsCurrentPositionValid(AActor* GroupTarget, AActor* AIGroupControlledActor) const;

	UFUNCTION(BlueprintCallable, Category = "AIGroupController")
	bool IsCurrentDesiredLocationValid(AActor* GroupTarget, AActor* AIGroupControlledActor) const;


	UFUNCTION(BlueprintCallable, Category = "AIGroupController")
	float GetAngleGroupTargetForwardToAIGroupActor(AActor* GroupTarget, AActor* AIGroupControlledActor) const;

	UFUNCTION(BlueprintCallable, Category = "AIGroupController")
	float GetAngleGroupTargetRightToAIGroupActor(AActor* GroupTarget, AActor* AIGroupControlledActor) const;


	UFUNCTION(BlueprintCallable, Category = "AIGroupController")
	void SetDestinationInField(AActor* GroupTarget, AActor* AIGroupControlledActor) const;

	UFUNCTION(BlueprintCallable, Category = "AIGroupController")
	void SetDestinationGroupTarget(AActor* GroupTarget, AActor* AIGroupControlledActor) const;

	UFUNCTION(BlueprintCallable, Category = "AIGroupController")
	FAIGroupControlData GetActorGroupData(AActor* AIGroupControlledActor) const;


	void SetAssignedGroupField(AActor* AIGroupControlledActor, EGroupField NewGroupField) const;


	UFUNCTION(BlueprintCallable, Category = "AIGroupController")
	void TryTriggerAttack(AActor* GroupTarget, AActor* AIGroupControlledActor);

	UFUNCTION(BlueprintCallable, Category = "AIGroupController")
	void SetGroupTargetForAIGroupActor(AActor* GroupTarget, AActor* AIGroupControlledActor);

	UFUNCTION(BlueprintCallable, Category = "AIGroupController")
	void RegisterGroupTargetWithAIGroupActor(AActor* GroupTarget, AActor* AIGroupControlledActor, EGroupField Feild);

	void GroupTargetFieldEntered(AActor* GroupTarget, AActor* AIGroupControlledActor, EGroupField EnteredField, EGroupField PreviousField);


	UFUNCTION(BlueprintImplementableEvent, Category = "AIGroupController | Events")
	void BP_GroupTargetFarFieldEntered(AActor* GroupTarget, AActor* AAIGroupController, FAIGroupControlData ControlledActorData, EGroupField PreviousField);

	UFUNCTION(BlueprintImplementableEvent, Category = "AIGroupController | Events")
	void BP_GroupTargetNearFieldEntered(AActor* GroupTarget, AActor* AAIGroupController, FAIGroupControlData ControlledActorData, EGroupField PreviousField);

	UFUNCTION(BlueprintImplementableEvent, Category = "AIGroupController | Events")
	void BP_GroupTargetActorFieldEntered(AActor* GroupTarget, AActor* AAIGroupController, FAIGroupControlData ControlledActorData, EGroupField PreviousField);

	UFUNCTION(BlueprintImplementableEvent, Category = "AIGroupController | Events")
	void BP_GroupTargetNoneFieldEntered(AActor* GroupTarget, AActor* AAIGroupController, FAIGroupControlData ControlledActorData, EGroupField PreviousField);



public:	

	// Sets default values for this actor's properties
	AAIGroupController();

	// Called every frame
	virtual void Tick(float DeltaTime) override;



	void OnGroupTargetFieldEnteredEvent(AActor* GroupTarget, AActor* AIGroupControlledActor, EGroupField EnteredField);



	void OnGroupTargetFieldVacatedEvent(AActor* GroupTarget, AActor* AIGroupControlledActor, EGroupField VacatedField);
	

	
	FORCEINLINE float GetActorFieldRadius() const { return ActorFieldRadius; }

	FORCEINLINE float GetNearFieldRadius() const { return NearFieldRadius; }

	FORCEINLINE float GetFarFieldRadius() const { return FarFieldRadius; }
	
	

};
