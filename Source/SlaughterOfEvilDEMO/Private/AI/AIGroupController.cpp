// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AIGroupController.h"

// Engine Includes
#include "DrawDebugHelpers.h"
#include "Math/Matrix.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Game Includes
#include "Player/SPlayerBase.h"
#include "Player/SEnemyBase.h"
#include "Controllers/SAIControllerBase.h"
#include "AI/SPlayerAIGroupSensor.h"
#include "AI/SAIGroupControlledActor.h"
#include "AI/AIGroupControlComponent.h"




// Sets default values
AAIGroupController::AAIGroupController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GroupTimerPeriod = 1.f;
	NearFieldRadius = 600.f;
	FarFieldRadius = 1000.f;

	NearFieldMaxActorCount = 2;
	FarFieldMaxActorCount = 5;

}


// Called when the game starts or when spawned
void AAIGroupController::BeginPlay()
{
	Super::BeginPlay();
	



	// Check valid values
	if (ActorFieldRadius >= NearFieldRadius)
	{
		UE_LOG(LogTemp, Error, TEXT("AIGroupController: ActorFieldRadius should be less than NearFieldRadius, expect unexpected behavior"));
	}
	if (NearFieldRadius >= FarFieldRadius)
	{
		UE_LOG(LogTemp, Error, TEXT("AIGroupController: NearFieldRadius should be less than FarFieldRadius, expect unexpected behavior"));
	}

	HalfNearFieldAttackFOV = NearFieldAttackFOV / 2.f;
	HalfNearFieldMaxFOV = NearFieldMaxFOV / 2.f;

	HalfFarFieldAttackFOV = FarFieldAttackFOV / 2.f;
	HalfFarFieldMaxFOV = FarFieldMaxFOV / 2.f;

	if (GroupTimerPeriod > 0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(GroupTickTimerHandle, this, &AAIGroupController::OnGroupTimerTick, GroupTimerPeriod, true);
	}
}


// Called every frame
void AAIGroupController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (DrawActorFieldDebug || DrawNearFieldDebug || DrawFarFieldDebug)
	{
		for (auto It = AIControlledGroups.CreateConstIterator(); It; ++It)
		{
			DrawDebug(It->Key);
		}
	}
}


void AAIGroupController::OnGroupTimerTick()
{

	for (auto AIControlledGroupsIt = AIControlledGroups.CreateConstIterator(); AIControlledGroupsIt; ++AIControlledGroupsIt)
	{
		AActor* GroupTarget = AIControlledGroupsIt->Key;
		if (GetNumberOfAIGroupActorsAssignedToField(GroupTarget, EGroupField::EGF_Near) > NearFieldMaxActorCount)
		{
			// Move farthest actor away from  GroupTarget to FarField
			AActor* FarthestActor = GetFarthestGroupControlledActorInField(GroupTarget, EGroupField::EGF_Near);
			AssignGroupField(FarthestActor, EGroupField::EGF_Far);
		}

		if (GetNumberOfAIGroupActorsAssignedToField(GroupTarget, EGroupField::EGF_Far) > FarFieldMaxActorCount)
		{
			AActor* FarthestActor = GetFarthestGroupControlledActorInField(GroupTarget, EGroupField::EGF_Far);
			AssignGroupField(FarthestActor, EGroupField::EGF_None);
		}

		for(auto& GroupTargetActor : AIControlledGroupsIt->Value.GroupActors)
		{
			SetDestinationInField(GroupTarget, GroupTargetActor);
		}
	}


	BP_OnGroupTimerTick();
}


FVector AAIGroupController::GetDesiredLocationMaxSeparation(AActor* GroupTarget, AActor* AIGroupControlledActor) const
{
	
	if (!GroupTarget || !AIGroupControlledActor) return FVector::ZeroVector;

	FVector NewDesiredLocation = AIGroupControlledActor->GetActorLocation();
	float FOVRangeLeft, FOVRangeRight, FieldRadius;
	FAIGroupControlData GroupControlData = GetActorGroupData(AIGroupControlledActor);

	//EGroupField EnemyGroup = GroupControlData.AssignedGroupField;  //GetGroupFeildAssignedToEnemy(Enemy);

	if (GroupControlData.AssignedGroupField == EGroupField::EGF_Near)
	{
		FOVRangeLeft = HalfNearFieldMaxFOV + 180.f;
		if (FOVRangeLeft > 360.f) FOVRangeLeft -= 360.f;

		FOVRangeRight = -HalfNearFieldMaxFOV + 180.f;
		if (FOVRangeRight > 360.f) FOVRangeRight -= 360.f;

		FieldRadius = NearFieldRadius;
	}
	else if (GroupControlData.AssignedGroupField == EGroupField::EGF_Far)
	{
		FOVRangeLeft = HalfFarFieldMaxFOV + 180.f;
		if (FOVRangeLeft > 360.f) FOVRangeLeft -= 360.f;

		FOVRangeRight = -HalfFarFieldMaxFOV + 180.f;
		if (FOVRangeRight > 360.f) FOVRangeRight -= 360.f;

		FieldRadius = FarFieldRadius;
	}
	else
	{
		return FVector::ZeroVector;
	}

	auto Target = AIControlledGroups.Find(GroupTarget);
	if (Target)
	{
		TArray<float> DesiredPositionAngles;
		DesiredPositionAngles.Add(FOVRangeLeft);
		DesiredPositionAngles.Add(FOVRangeRight);

		for (auto ActorInGroup : Target->GroupActors)
		{
			if (ActorInGroup != AIGroupControlledActor)
			{
				
				float angle = GetAngleGroupTargetRightVectorToTargetLocation(GroupTarget, GroupControlData.DesiredLocation) - 270.f;
			
				if (angle <= 0.f) angle += 360.f;

				if (angle > FOVRangeRight && angle < FOVRangeLeft)
				{
					DesiredPositionAngles.Add(angle);
				}
			}
		}

		DesiredPositionAngles.Sort([](const float& a, const float& b) {return a < b;});

		float MaxAngleBetweenDesiredLocations = 0.f;
		float MaxAngleIndex = 0;

		for (int32 i = 1; i < DesiredPositionAngles.Num(); ++i)
		{
			float AngleBetweenDesiredLocations = DesiredPositionAngles[i] - DesiredPositionAngles[i-1];
			if (AngleBetweenDesiredLocations > MaxAngleBetweenDesiredLocations)
			{
				MaxAngleBetweenDesiredLocations = AngleBetweenDesiredLocations;
				MaxAngleIndex = i;
			}
		}

		float MaxAngleMidPoint = ((DesiredPositionAngles[MaxAngleIndex] - DesiredPositionAngles[MaxAngleIndex - 1]) / 2.f) + DesiredPositionAngles[MaxAngleIndex - 1];
		MaxAngleMidPoint -= 180.f;
		if (MaxAngleMidPoint < 0.f) MaxAngleMidPoint += 360.f;

		NewDesiredLocation = GroupTarget->GetActorLocation() + PointOnCircle(GroupTarget->GetActorRotation().Yaw - MaxAngleMidPoint, FieldRadius);

	}

	return NewDesiredLocation;

}


void AAIGroupController::OnGroupTargetFieldEnteredEvent(AActor* GroupTarget, AActor* AIGroupControlledActor, EGroupField EnteredField)
{
	if (!GroupTarget || !AIGroupControlledActor) return;

	EGroupField PreviousField = IncreaseFieldPresence(AIGroupControlledActor);
	GroupTargetFieldEntered(GroupTarget, AIGroupControlledActor, EnteredField, PreviousField);
}


void AAIGroupController::OnGroupTargetFieldVacatedEvent(AActor* GroupTarget, AActor* AIGroupControlledActor, EGroupField VacatedField)
{
	if (!GroupTarget || !AIGroupControlledActor) return;

	DecreaseFieldPresence(AIGroupControlledActor);
	EGroupField CurrentField = GetActorGroupData(AIGroupControlledActor).CurrentGroupField;
	GroupTargetFieldEntered(GroupTarget, AIGroupControlledActor, CurrentField, VacatedField);
}


void AAIGroupController::GroupTargetFieldEntered(AActor* GroupTarget, AActor* AIGroupControlledActor, EGroupField EnteredField, EGroupField PreviousField)
{
	if (!GroupTarget || !AIGroupControlledActor) return;

	FString EnteredFieldNameString = EnumToString(TEXT("EGroupField"), static_cast<uint8>(EnteredField));
	FString PreviousFieldNameString = EnumToString(TEXT("EGroupField"), static_cast<uint8>(PreviousField));

	UE_LOG(LogTemp, Warning, TEXT("AAIGroupController::GroupTargetFieldEntered - Entered = (%s), Previous = (%s)"), *EnteredFieldNameString, *PreviousFieldNameString);

	if (EnteredField == EGroupField::EGF_Near || EnteredField == EGroupField::EGF_Far)
	{
		FAIControlledGroup* GroupTargetControlGroupData = AIControlledGroups.Find(GroupTarget);
		if (GroupTargetControlGroupData && EnteredField == EGroupField::EGF_Near)
		{
			GroupTargetControlGroupData->CurrentNearFieldActorCount++;
		}
		else if (GroupTargetControlGroupData && EnteredField == EGroupField::EGF_Far)
		{
			GroupTargetControlGroupData->CurrentFarFieldActorCount++;
		}
	}

	switch (EnteredField)
	{
		case EGroupField::EGF_None:
			BP_GroupTargetNoneFieldEntered(GroupTarget, AIGroupControlledActor, GetActorGroupData(AIGroupControlledActor), PreviousField);
			break;
		case EGroupField::EGF_Far:
			BP_GroupTargetFarFieldEntered(GroupTarget, AIGroupControlledActor, GetActorGroupData(AIGroupControlledActor), PreviousField);
			break;
		case EGroupField::EGF_Near:
			BP_GroupTargetNearFieldEntered(GroupTarget, AIGroupControlledActor, GetActorGroupData(AIGroupControlledActor), PreviousField);
			break;
		case EGroupField::EGF_Actor:
			BP_GroupTargetActorFieldEntered(GroupTarget, AIGroupControlledActor, GetActorGroupData(AIGroupControlledActor), PreviousField);
			break;
		default:
			break;
	}
}


void AAIGroupController::RegisterGroupTargetWithAIGroupActor(AActor* GroupTarget, AActor* AIGroupControlledActor, EGroupField Field)
{
	if (!GroupTarget || !AIGroupControlledActor) return;


	// Find the AIControlledGroup for GroupTarget, add if no AIControlledGroup is assined to GroupTarget 
	FAIControlledGroup* GroupTargetAIControlledGroup = AIControlledGroups.Find(GroupTarget);
	if (!GroupTargetAIControlledGroup)
	{
		AIControlledGroups.Add(GroupTarget);
		GroupTargetAIControlledGroup = AIControlledGroups.Find(GroupTarget);
	}
    
	// Get the TargetActor for AIGroupControlledActor
	AActor* AIGroupControlledActorTargetActor = GetActorGroupData(AIGroupControlledActor).TargetActor;

	// if the AIGroupControlledActor's current target actor is the GroupTarget, then make sure AIGroupControlledActor
	// is in GroupTarget's AIControlledGoup.GroupActors 
	if (AIGroupControlledActorTargetActor == GroupTarget)
	{
		if (GroupTargetAIControlledGroup && GroupTargetAIControlledGroup->GroupActors.Find(AIGroupControlledActorTargetActor) == nullptr)
		{
			GroupTargetAIControlledGroup->GroupActors.Add(AIGroupControlledActor);
		}
	}

	// If AIGroupControlledActor current target actor is null then add GroupTarget to its target actor. Then add
	// AIGroupControlledActor ti GroupTargets AIControlledGroup.GroupActirs
	else if (AIGroupControlledActorTargetActor == nullptr)
	{
		SetGroupTargetForAIGroupActor(GroupTarget, AIGroupControlledActor);

		if (GroupTargetAIControlledGroup)
		{
			GroupTargetAIControlledGroup->GroupActors.Add(AIGroupControlledActor);
		}
		
		//SetDestinationInField(GroupTarget, AIGroupControlledActor);
	}

	// AIGroupControlledActor's TargetActor is different then GroupTarget. Remove AIGroupControlledActor from
	// its current TargetActors AIControlledGroup.GroupActors TSet. Then add AIGroupControlledActor to GroupTargets
	// AIControlledGroup.GroupActors TSet
	else // AIGroupControlledActor is already assigned to another group
	{
		FAIControlledGroup* a = AIControlledGroups.Find(AIGroupControlledActorTargetActor);
		if (a)
		{
			a->GroupActors.Remove(AIGroupControlledActor);
		}

		if (GroupTargetAIControlledGroup)
		{
			GroupTargetAIControlledGroup->GroupActors.Add(AIGroupControlledActor);
		}
	}


	AssignGroupField(AIGroupControlledActor, Field);
}


int32 AAIGroupController::GetNumberOfAIGroupActorsAssignedToField(AActor* GroupTarget, EGroupField GroupField)
{

	int count = 0;
	if (!GroupTarget) return count;

	FAIControlledGroup* AIControlGroup = AIControlledGroups.Find(GroupTarget);
	if (AIControlGroup)
	{
		for(auto& Actor : AIControlGroup->GroupActors)
		{
			if (GetActorGroupData(Actor).AssignedGroupField == GroupField)
			{
				count++;
			}
		}
	}

	return count;
}


AActor* AAIGroupController::GetFarthestGroupControlledActorInField(AActor* GroupTarget, EGroupField Field)
{
	FAIControlledGroup* AIControlGroup = AIControlledGroups.Find(GroupTarget);
	if (AIControlGroup)
	{	
		float MaxDistance = 0.f;
		AActor* MaxDistanceActor = nullptr;
		for(auto& Actor : AIControlGroup->GroupActors)
		{
			FAIGroupControlData ControlData = GetActorGroupData(Actor);
			if (ControlData.CurrentGroupField == Field)
			{
				float Distance = (Actor->GetActorLocation() - GroupTarget->GetActorLocation()).SizeSquared();
				if (Distance > MaxDistance)
				{
					MaxDistance = Distance;
					MaxDistanceActor = Actor;
				}
			}
		}

		return MaxDistanceActor;
	}

	return nullptr;
}


bool AAIGroupController::IsAllowedInNearFeild(AActor* AIGroupControlledActor) const
{
	if (!AIGroupControlledActor) return false;

	FAIGroupControlData AIGroupControlData = GetActorGroupData(AIGroupControlledActor);
	return (AIGroupControlData.AssignedGroupField == EGroupField::EGF_Near);
}


EValidFieldResult AAIGroupController::InValidFieldRange(AActor* GroupTarget, AActor* AIGroupControlledActor) const
{
	if (!GroupTarget || !AIGroupControlledActor) return EValidFieldResult::EVFR_InValidRange;

	FAIGroupControlData AIGroupControlData = GetActorGroupData(AIGroupControlledActor);

	float Distance = (AIGroupControlledActor->GetActorLocation() - GroupTarget->GetActorLocation()).Size();

	if (AIGroupControlData.AssignedGroupField == EGroupField::EGF_Near)
	{
		if (Distance >= ActorFieldRadius && Distance <= NearFieldRadius)
		{
			return EValidFieldResult::EVFR_InValidRange;
		}
		else if (Distance < ActorFieldRadius)
		{
			return EValidFieldResult::EVFR_LessThanMin;
		}
		else // PlayerToEnemyDistance > NearFieldRadius
		{
			return EValidFieldResult::EVFR_GreaterThanMax;
		}
	}
	else if (AIGroupControlData.AssignedGroupField == EGroupField::EGF_Far)
	{
		if (Distance >= NearFieldRadius && Distance <= FarFieldRadius)
		{
			return EValidFieldResult::EVFR_InValidRange;
		}
		else if (Distance < NearFieldRadius)
		{
			return EValidFieldResult::EVFR_LessThanMin;
		}
		else // PlayerToEnemyDistance > FarFieldRadius
		{
			return EValidFieldResult::EVFR_GreaterThanMax;
		}
	}

	return EValidFieldResult::EVFR_InValidRange;
}


bool AAIGroupController::InValidFOV(AActor* GroupTarget, AActor* AIGroupControlledActor, EFOVRange FOVRange) const
{
	if (!GroupTarget || !AIGroupControlledActor) return false;

	//EGroupField GroupFeild = GetGroupFeildAssignedToEnemy(Enemy);
	FAIGroupControlData AIGroupControlData = GetActorGroupData(AIGroupControlledActor);

	float Angle = GetAngleGroupTargetForwardToAIGroupActor(GroupTarget, AIGroupControlledActor);

	


	if (AIGroupControlData.AssignedGroupField == EGroupField::EGF_Near)
	{
		float FOV = (FOVRange == EFOVRange::EFR_MaxFOV) ? HalfNearFieldMaxFOV : HalfNearFieldAttackFOV;
		return (Angle <= FOV || Angle >= (360.f - FOV));
	}
	else if (AIGroupControlData.AssignedGroupField == EGroupField::EGF_Far)
	{
		float FOV = (FOVRange == EFOVRange::EFR_MaxFOV) ? HalfFarFieldMaxFOV : HalfFarFieldAttackFOV;
		return (Angle <= FOV || Angle >= (360.f - FOV));
	}

	return true;
}


bool AAIGroupController::IsCurrentPositionValid(AActor* GroupTarget, AActor* AIGroupControlledActor) const
{
	if (!GroupTarget || !AIGroupControlledActor) return false;

	return (InValidFOV(GroupTarget, AIGroupControlledActor, EFOVRange::EFR_MaxFOV) && InValidFieldRange(GroupTarget, AIGroupControlledActor) == EValidFieldResult::EVFR_InValidRange);
}


bool AAIGroupController::IsCurrentDesiredLocationValid(AActor* GroupTarget, AActor* AIGroupControlledActor) const
{
	if (!GroupTarget || !AIGroupControlledActor) return false;

	FAIGroupControlData AIGroupControlData = GetActorGroupData(AIGroupControlledActor);

	// check if distance is valid
	float Distance = (AIGroupControlData.DesiredLocation - GroupTarget->GetActorLocation()).Size();
	float Angle = GetAngleGroupTargetRightVectorToTargetLocation(GroupTarget, AIGroupControlData.DesiredLocation);

	// Convert angle from player right vector to forward vector
	Angle -= 90.f;
	if (Angle < 0.f) Angle += 360.f;

	if (AIGroupControlData.AssignedGroupField == EGroupField::EGF_Near)
	{
		return (Distance >= ActorFieldRadius && Distance <= NearFieldRadius &&
			(Angle <= HalfNearFieldMaxFOV || Angle >= (360.f - HalfNearFieldMaxFOV)));
	}
	else if (AIGroupControlData.AssignedGroupField == EGroupField::EGF_Far)
	{
		return (Distance >= NearFieldRadius && Distance <= FarFieldRadius &&
			(Angle <= HalfFarFieldMaxFOV || Angle >= (360.f - HalfFarFieldMaxFOV)));
	}

	return false;
}


float AAIGroupController::GetAngleGroupTargetForwardToAIGroupActor(AActor* GroupTarget, AActor* AIGroupControlledActor) const
{
	if (!GroupTarget || !AIGroupControlledActor) return NAN;

	float angle = GetAngleGroupTargetRightVectorToTargetLocation(GroupTarget, AIGroupControlledActor->GetActorLocation());
	angle -= 90.f;

	if (angle < 0.f)
	{
		angle += 360.f;
	}

	return angle;
}


float AAIGroupController::GetAngleGroupTargetRightToAIGroupActor(AActor* GroupTarget, AActor* AIGroupControlledActor) const
{
	if (!GroupTarget || !AIGroupControlledActor) return NAN;

	float angle = GetAngleGroupTargetRightVectorToTargetLocation(GroupTarget, AIGroupControlledActor->GetActorLocation());

	return angle;
	/*FVector EnemyToPlayer = Enemy->GetActorLocation() - Player->GetActorLocation();
	float dx = FVector::DotProduct(EnemyToPlayer, Player->GetActorForwardVector());
	float dy = FVector::DotProduct(EnemyToPlayer, Player->GetActorRightVector());
	float angle = FMath::RadiansToDegrees(FMath::Atan2(dx, dy));

	if (angle < 0.f)
	{
		angle += 360.f;
	}

	return angle;*/
}


void AAIGroupController::SetDestinationInField(AActor* GroupTarget, AActor* AIGroupControlledActor) const
{
	if (!GroupTarget || !AIGroupControlledActor) return;

	//auto EnemyController = Cast<ASAIControllerBase>(Enemy->GetController());
	
	FVector MoveToLocation = GetDesiredLocationMaxSeparation(GroupTarget, AIGroupControlledActor);

	UAIGroupControlComponent* AIGroupControlComponent = GetAIGroupControlComponent(AIGroupControlledActor);
	if (AIGroupControlComponent)
	{
		AIGroupControlComponent->GroupControllerData.DesiredLocation = MoveToLocation;
	}

	UKismetSystemLibrary::DrawDebugSphere(GetWorld(), MoveToLocation, 10.f, 12, FColor::White, 5.f, 2.f);
}


void AAIGroupController::SetDestinationGroupTarget(AActor* GroupTarget, AActor* AIGroupControlledActor) const
{
	if (!GroupTarget || !AIGroupControlledActor) return;

	UAIGroupControlComponent* AIGroupControlComponent = GetAIGroupControlComponent(AIGroupControlledActor);
	if (AIGroupControlComponent)
	{
		AIGroupControlComponent->GroupControllerData.DesiredLocation = GroupTarget->GetActorLocation();
	}
}


FAIGroupControlData AAIGroupController::GetActorGroupData(AActor* AIGroupControlledActor) const
{	
	FAIGroupControlData GroupControlData;

	if (AIGroupControlledActor)
	{
		auto GroupControlComp = AIGroupControlledActor->FindComponentByClass<UAIGroupControlComponent>();
		if (GroupControlComp)
		{
			return GroupControlComp->GroupControllerData;
		}
	}

	return GroupControlData;
}


void AAIGroupController::SetAssignedGroupField(AActor* AIGroupControlledActor, EGroupField NewGroupField) const
{
	if (!AIGroupControlledActor) return;

	UAIGroupControlComponent* AIGroupControlComponent = GetAIGroupControlComponent(AIGroupControlledActor);
	if (AIGroupControlComponent)
	{
		AIGroupControlComponent->GroupControllerData.AssignedGroupField = NewGroupField;
	}
}


void AAIGroupController::AssignGroupField(AActor* AIGroupControlledActor, EGroupField GroupField) const
{
	if (!AIGroupControlledActor) return;

	UAIGroupControlComponent* AIGroupControlComponent = GetAIGroupControlComponent(AIGroupControlledActor);
	if (AIGroupControlComponent)
	{
		AIGroupControlComponent->GroupControllerData.AssignedGroupField = GroupField;

		FString GroupFieldNameString = EnumToString(TEXT("EGroupField"), static_cast<uint8>(GroupField));
		UE_LOG(LogTemp, Warning, TEXT("%s is now is GroupTarget's %s Field"), *AIGroupControlComponent->GetName(), * GroupFieldNameString);
	}
}


EGroupField AAIGroupController::IncreaseFieldPresence(AActor* AIGroupControlledActor)
{
	if (!AIGroupControlledActor) return EGroupField::EGF_None;

	UAIGroupControlComponent* AIGroupControlComponent = GetAIGroupControlComponent(AIGroupControlledActor);
	if (AIGroupControlComponent && AIGroupControlComponent->GroupControllerData.CurrentGroupField != EGroupField::EGF_Actor)
	{
		uint8 PreviousField = (uint8)AIGroupControlComponent->GroupControllerData.CurrentGroupField;
		AIGroupControlComponent->GroupControllerData.CurrentGroupField = (EGroupField)(PreviousField + 1);
		return (EGroupField)PreviousField;
	}

	return EGroupField::EGF_None;
}


EGroupField AAIGroupController::DecreaseFieldPresence(AActor* AIGroupControlledActor)
{
	if (!AIGroupControlledActor) return EGroupField::EGF_None;

	UAIGroupControlComponent* AIGroupControlComponent = GetAIGroupControlComponent(AIGroupControlledActor);
	if (AIGroupControlComponent && AIGroupControlComponent->GroupControllerData.CurrentGroupField != EGroupField::EGF_None)
	{
		uint8 PreviousField = (uint8)AIGroupControlComponent->GroupControllerData.CurrentGroupField;
		AIGroupControlComponent->GroupControllerData.CurrentGroupField = (EGroupField)(PreviousField - 1);
		return (EGroupField)PreviousField;
	}

	return EGroupField::EGF_None;
}


UAIGroupControlComponent* AAIGroupController::GetAIGroupControlComponent(AActor* AIGroupControlledActor) const
{
	if (!AIGroupControlledActor) return nullptr;

	return AIGroupControlledActor->FindComponentByClass<UAIGroupControlComponent>();
}


void AAIGroupController::SetGroupTargetForAIGroupActor(AActor* GroupTarget, AActor* AIGroupControlledActor)
{
	if (!GroupTarget || !AIGroupControlledActor) return;

	UE_LOG(LogTemp, Warning, TEXT("I am here"));
	UAIGroupControlComponent* AIGroupControlComponent = GetAIGroupControlComponent(AIGroupControlledActor);
	if (AIGroupControlComponent)
	{
		AIGroupControlComponent->GroupControllerData.TargetActor = GroupTarget;
	}
}


void AAIGroupController::DrawDebug(AActor* GroupTarget)
{
	if (!GroupTarget) return;

	FVector Location = GroupTarget->GetActorLocation();
	FVector ForwardVector = GroupTarget->GetActorForwardVector();
	FRotator Rotation = GroupTarget->GetActorRotation();
	FVector DrawDebugOffset = FVector::ZeroVector;
	
	auto GroupTargetCharacter = Cast<ACharacter>(GroupTarget);
	if (GroupTargetCharacter)
	{
		DrawDebugOffset = FVector(Location.X, Location.Y, Location.Z - GroupTargetCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	}
	
	if (DrawNearFieldDebug)
	{
		// Draw cylinder encompassing near field radius 
		DrawDebugCylinder(GetWorld(), DrawDebugOffset, DrawDebugOffset + FVector(0,0,200.f), NearFieldRadius, 20, NearFieldDebugColor, false, -1.f, 0, 3.f);
		
		// Draw near field Max fov markers
		FVector RightNearFieldEndPoint = Location + PointOnCircle(Rotation.Yaw + HalfNearFieldMaxFOV, NearFieldRadius);
		FVector LeftNearFieldEndPoint = Location + PointOnCircle(Rotation.Yaw - HalfNearFieldMaxFOV, NearFieldRadius);
		DrawDebugLine(GetWorld(), Location + ForwardVector, RightNearFieldEndPoint, NearFieldDebugColor, false, -1.f, 0, 3.f);
		DrawDebugLine(GetWorld(), Location + ForwardVector, LeftNearFieldEndPoint, NearFieldDebugColor, false, -1.f, 0, 3.f);
	}

	if (DrawFarFieldDebug)
	{
		// Draw cylinder encompassing far field radius 
		DrawDebugCylinder(GetWorld(), DrawDebugOffset, DrawDebugOffset + FVector(0, 0, 200.f), FarFieldRadius, 30, FarFieldDebugColor, false, -1.f, 0, 3.f);

		// Draw far field Max fov markers
		FVector RightFarFieldEndPoint = Location + PointOnCircle(Rotation.Yaw + HalfFarFieldMaxFOV, FarFieldRadius);
		FVector LeftFarFieldEndPoint = Location + PointOnCircle(Rotation.Yaw - HalfFarFieldMaxFOV, FarFieldRadius);
		DrawDebugLine(GetWorld(), Location + ForwardVector, RightFarFieldEndPoint, FarFieldDebugColor, false, -1.f, 0, 3.f);
		DrawDebugLine(GetWorld(), Location + ForwardVector, LeftFarFieldEndPoint, FarFieldDebugColor, false, -1.f, 0, 3.f);
	}

	if (DrawActorFieldDebug)
	{
		DrawDebugCylinder(GetWorld(), DrawDebugOffset, DrawDebugOffset + FVector(0, 0, 200.f), ActorFieldRadius, 16, ActorFieldDebugColor, false, -1.f, 1.f);
	}
}


const FString AAIGroupController::EnumToString(const TCHAR* Enum, int32 EnumValue) const
{
	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, Enum, true);
	if (!EnumPtr)
	{
		return NSLOCTEXT("Invalid", "Invalid", "Invalid").ToString();
	}

#if WITH_EDITOR
	return EnumPtr->GetDisplayNameTextByIndex(EnumValue).ToString();
#else
	return EnumPtr->GetEnumName(EnumValue);
#endif
}


FVector AAIGroupController::PointOnCircle(float Angle, float DistanceFromCenter) const
{
	return FVector((FMath::Cos(FMath::DegreesToRadians(Angle)) * DistanceFromCenter), (FMath::Sin(FMath::DegreesToRadians(Angle)) * DistanceFromCenter), 0.f );
}


float AAIGroupController::GetAngleGroupTargetRightVectorToTargetLocation(AActor* GroupTarget, FVector TargetLocation) const
{
	if (!GroupTarget) return 0.f;

	FVector TargetLocationToPlayer = TargetLocation - GroupTarget->GetActorLocation();
	float dx = FVector::DotProduct(TargetLocationToPlayer, GroupTarget->GetActorForwardVector());
	float dy = FVector::DotProduct(TargetLocationToPlayer, GroupTarget->GetActorRightVector());
	float angle = FMath::RadiansToDegrees(FMath::Atan2(dx, dy));

	if (angle < 0.f)
	{
		angle += 360.f;
	}

	return angle;
}


