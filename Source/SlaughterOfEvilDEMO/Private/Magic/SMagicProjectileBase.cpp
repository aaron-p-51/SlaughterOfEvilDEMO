// Fill out your copyright notice in the Description page of Project Settings.


#include "Magic/SMagicProjectileBase.h"

// Engine Includes
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/ProjectileMovementComponent.h"


// Game Includes
#include "SMeleeWeaponWielder.h"
#include "Weapons/SMeleeWeaponBase.h"
#include "Components/SMagicChargeComponent.h"
#include "SProjectileMagicTarget.h"

// Sets default values
ASMagicProjectileBase::ASMagicProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	if (SphereComp)
	{
		SetRootComponent(SphereComp);
	}

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	if (SphereComp)
	{
		MeshComp->SetupAttachment(GetRootComponent());
	}

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));

	// Set defaults (if errors are found try setting in BeginPlay()
	SphereTraceRadius = 10.f;
	BaseDamage = 0.f;
	bCanBeBlocked = false;
	bCausesDamage = false;
	CurrentPosition = FVector::ZeroVector;
	PreviousPosition = FVector::ZeroVector;
	LifeTime = 4.f;
	StopHomingDistance = 0.f;
	bReplicates = true;

	// Let clients update movement themselves
	SetReplicateMovement(false);
}


// Called when the game starts or when spawned
void ASMagicProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	// Set timer for LifeTime
	GetWorldTimerManager().SetTimer(TimerHandle_LifeTime, this, &ASMagicProjectileBase::LifeTimeExceeded, LifeTime);

	// Sphere trace radius for collision detection will be radius of SphereComp
	if (SphereComp)
	{
		SphereTraceRadius = SphereComp->GetScaledSphereRadius();
	}

	PreviousPosition = GetActorLocation();

	StopHomingDistanceSquared = StopHomingDistance * StopHomingDistance;
}


// Called every frame
void ASMagicProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Hit detection is done from the actors position last frame and this current frame, get position this frame
	CurrentPosition = GetActorLocation();

	TArray<FHitResult> HitResults;
	if (DetectHit(HitResults))
	{
		EmitOnHitEffects(HitResults);

		if (GetLocalRole() == ENetRole::ROLE_Authority)
		{
			TryApplyMagicCharge(HitResults);

			// If hit is detected on server before clients then stop replication, allow for clients to 
			// show particle effects from collision
			TearOff();
		}
		
		Destroy();
	}

	if (StopHomingDistance > 0.f)
	{
		StopHomingDistanceCheckAndApply();
	}

	PreviousPosition = GetActorLocation();
}


void ASMagicProjectileBase::StopHomingDistanceCheckAndApply()
{
	USceneComponent* HomingTarget = Cast<USceneComponent>(ProjectileMovement->HomingTargetComponent);
	if (HomingTarget)
	{
		float DistanceSquared = (HomingTarget->GetOwner()->GetActorLocation() - GetActorLocation()).SizeSquared();
		if (DistanceSquared >= StopHomingDistanceSquared)
		{
			ProjectileMovement->HomingTargetComponent = nullptr;
		}
	}
}


bool ASMagicProjectileBase::DetectHit(TArray<FHitResult>& HitResults) const
{
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(GetOwner());
	IgnoreActors.Add(GetInstigator());

	// Sphere trace from location last from to location this frame
	bool bHitDetected = UKismetSystemLibrary::SphereTraceMulti(
		this,
		PreviousPosition,
		CurrentPosition,
		SphereTraceRadius,
		UEngineTypes::ConvertToTraceType(CollisionChannel),
		false,
		IgnoreActors,
		EDrawDebugTrace::None,
		HitResults,
		true
	);


	return bHitDetected;
}


void ASMagicProjectileBase::EmitOnHitEffects(const TArray<FHitResult>& HitResults) const
{
	if (OnHitEffects)
	{
		// Multiple collisions may have been detected. Get GetFirstHitActorHitResult will get HitResult that occurred first and where the OnHitEffects should occur
		FHitResult FirstHitActorResult = GetFirstHitActorHitResult(HitResults);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OnHitEffects, FirstHitActorResult.Location, UKismetMathLibrary::MakeRotFromX(FirstHitActorResult.Normal));
	}
}


FHitResult ASMagicProjectileBase::GetFirstHitActorHitResult(const TArray<FHitResult>& HitResults) const
{
	float ClosestDistance = NAN;
	FHitResult Result;

	// Look at all actors in HitResults. Find the Actor that has the closest location this actors location last frame.
	for (auto HitResult : HitResults)
	{
		auto HitLocation = HitResult.Location;

		float DistanceToHitHitLocation = FVector::DistSquared(HitLocation, PreviousPosition);
		if (ClosestDistance == NAN || DistanceToHitHitLocation < ClosestDistance)
		{
			ClosestDistance = DistanceToHitHitLocation;
			Result = HitResult;	
		}
	}

	return Result;
}


void ASMagicProjectileBase::SetHomingTargetActor(AActor* TargetActor)
{
	if (GetLocalRole() == ENetRole::ROLE_Authority && ProjectileMovement &&
		ProjectileMovement->bIsHomingProjectile && TargetActor)
	{
		HomingActor = TargetActor;
		OnRep_HomingTargetActor();
	}
}


void ASMagicProjectileBase::OnRep_HomingTargetActor()
{
	if (!HomingActor) return;
	
	// Homing Actor must implement ISProjectileMagicTarget. Actors who ISProjectileMagicTarget will have a
	// a USceneComponent that can be used as a HomingTargetComponent. Actors who implement ISProjectileMagicTarget may chance
	// the relative transform of the component being used as the USceneComponent for example: player crouching and change 
	// HomingTargetComponent to change heights. 
	auto ProjectileTargetInterface = Cast<ISProjectileMagicTarget>(HomingActor);
	if (ProjectileTargetInterface && ProjectileMovement && ProjectileMovement->bIsHomingProjectile)
	{
		ProjectileMovement->HomingTargetComponent = ProjectileTargetInterface->GetProjectileHomingTarget();
	}
	
	//TODO: If actor does not implement ISProjectileMagicTarget dynamically create a USceneCompoent on actor
	// and assign HomingTargetComponent (TWeakObjectPtr) to it.
	
}


void ASMagicProjectileBase::LifeTimeExceeded()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_LifeTime);

	// Play OnHit Results if that is desired behavior
	if (bPlayOnHitEffectsLifeTimeExceeded && OnHitEffects)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OnHitEffects, GetActorLocation(), GetActorRotation(), true);
	}

	// If LifeTimeExceeded on server occurs before clients then stop replication, allow for clients to 
	// show particle effects from collision and destroy themselves
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		TearOff();
	}

	Destroy();
}


bool ASMagicProjectileBase::TryApplyMagicCharge(const TArray<FHitResult>& HitResults) const
{
	if (GetLocalRole() != ENetRole::ROLE_Authority) return false;

	for (auto& HitResult : HitResults)
	{
		// check if the hit actor has a USMagicChargeComponent if so try and apply a magic charge to it.
		auto ActorMagicChargeComp = HitResult.GetActor()->FindComponentByClass<USMagicChargeComponent>();
		if (ActorMagicChargeComp)
		{
			auto a = Cast<AActor>(this);
			bool AppliedCharge = ActorMagicChargeComp->TrySetMagicCharge(this);
		}	
		else
		{
				// Apply Damage...
		}
	}

	return false;
}


void ASMagicProjectileBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASMagicProjectileBase, HomingActor);
}


