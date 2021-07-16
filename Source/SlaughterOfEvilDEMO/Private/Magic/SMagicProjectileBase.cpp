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


// Game Includes
#include "SMeleeWeaponWielder.h"
#include "Weapons/SMeleeWeaponBase.h"
#include "Components/SMagicChargeComponent.h"

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


	// Set defaults (if errors are found try setting in BeginPlay()
	InitialSpeed = 10.f;
	Mass = 1.f;
	Drag = 0.f;
	SphereTraceRadius = 10.f;
	BaseDamage = 0.f;
	bCanBeBlocked = false;
	bCausesDamage = false;
	Velocity = FVector::ZeroVector;
	Acceleration = FVector::ZeroVector;
	Gravity = FVector::ZeroVector;
	NextPosition = FVector::ZeroVector;
	PreviousPosition = FVector::ZeroVector;

	bReplicates = true;

}


// Called when the game starts or when spawned
void ASMagicProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	if (SphereComp)
	{
		SphereTraceRadius = SphereComp->GetScaledSphereRadius();
	}

	// Set projectile initial velocity
	Velocity = GetActorForwardVector() * InitialSpeed;


	// Assume Gravity and Mass will not change, avoid computing every frame
	if (Mass == 0.f || FMath::IsNearlyZero(Mass))
	{
		DragEffect = 0.f;
	}
	else
	{
		DragEffect = Drag / Mass;
	}
}


// Called every frame
void ASMagicProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Movement is not replicated. All clients will move projectile on their own
	CalculateMovement(DeltaTime);

	// Hits will be detected on all clients, only server will apply damage and try to set magic charge state on hit actors
	if (DetectHit())
	{
		if (GetLocalRole() == ENetRole::ROLE_Authority)
		{
			TearOff();
		}

		Destroy();
	}
	else
	{
		SetActorLocation(NextPosition);
		SetActorRotation(UKismetMathLibrary::MakeRotFromX(Velocity));
	}
}


void ASMagicProjectileBase::CalculateMovement(float DeltaTime)
{
	PreviousPosition = GetActorLocation();

	// Calculate acceleration taking into account gravity and drag
	Acceleration = Gravity - (Velocity * Velocity) * DragEffect;

	// Calculate the position offset this frame (See https://www.meizenberg.com/post/projectiles-with-blueprints for
	// greater explanation, sources in post.)
	FVector PositionOffset = (Velocity * DeltaTime) + (Acceleration * DeltaTime * DeltaTime) / 2;

	// Position to move to this frame
	NextPosition = PreviousPosition + PositionOffset;

	// Update Velocity for next frame
	Velocity = Velocity + Acceleration * DeltaTime;
}


bool ASMagicProjectileBase::DetectHit()
{

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(GetOwner());

	TArray<FHitResult> HitResults;

	// Sphere trace from previous position to position projectile will move to at the end of this frame 
	bool bHitDetected = UKismetSystemLibrary::SphereTraceMulti(
		this,
		PreviousPosition,
		NextPosition,
		SphereTraceRadius,
		UEngineTypes::ConvertToTraceType(CollisionChannel),
		false,
		IgnoreActors,
		EDrawDebugTrace::None,
		HitResults,
		true
	);


	if (bHitDetected)
	{
		if (OnHitEffects)
		{
			FHitResult FirstHitActorResult = GetFirstHitActorHitResult(HitResults);
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OnHitEffects, FirstHitActorResult.Location, UKismetMathLibrary::MakeRotFromX(FirstHitActorResult.Normal));
		}

		if (GetLocalRole() == ENetRole::ROLE_Authority)
		{
			TryApplyMagicCharge(HitResults);
		}
		
	}


	return bHitDetected;
}


FHitResult ASMagicProjectileBase::GetFirstHitActorHitResult(TArray<FHitResult>& HitResults)
{
	float ClosestDistance = NAN;
	FHitResult Result;

	for (auto HitResult : HitResults)
	{
		auto HitLocation = HitResult.Location;

		float DistanceToHitHitLocation = FVector::DistSquared(HitLocation, GetActorLocation());
		if (ClosestDistance == NAN || DistanceToHitHitLocation < ClosestDistance)
		{
			ClosestDistance = DistanceToHitHitLocation;
			Result = HitResult;
			
		}
		
	}

	return Result;
}


//TODO: if hit multiple actors(player and longsword) if longsword has successful magic charge set do not apple damage to player
bool ASMagicProjectileBase::TryApplyMagicCharge(TArray<FHitResult>& HitResults)
{
	for (auto HitResult : HitResults)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitResult.GetActor()->GetName());
		auto ActorMagicChargeComp = HitResult.GetActor()->FindComponentByClass<USMagicChargeComponent>();
		if (ActorMagicChargeComp)
		{
			if (HitActorCanAcceptMagicCharge(*ActorMagicChargeComp))
			{
				bool AppliedCharge = ActorMagicChargeComp->TrySetMagicCharge(true);
			}
			else
			{
				// Apply Damage...
			}
		}
	}

	return false;
}


bool ASMagicProjectileBase::HitActorCanAcceptMagicCharge(USMagicChargeComponent& HitActorMagicChargeComp)
{
	if (HitActorMagicChargeComp.IsMagicCharged()) return false;

	if (HitActorMagicChargeComp.OwnerMustFaceChargeSource())
	{
		// Get the owner of the owner of HitActorMagicChargeComp, this should be CharacterBase 
		auto HitActorOwner = HitActorMagicChargeComp.GetOwner()->GetOwner();
		if (HitActorOwner)
		{
			// Get the dot product between projectile forward vector and forward vector of HitActors Owner 
			float Dot = FVector::DotProduct(GetActorForwardVector(), HitActorOwner->GetActorForwardVector());
		
			// The max angle HitActorOwner can face away from the projectile is 180 (deg). Map 0 to 180 range to
			// output range of dot product (for dot product -1 if actors in line above face same direction, 1 of opposite directions) 
			float MapDot = FMath::GetMappedRangeValueUnclamped(FVector2D(-1.f, 1.f), FVector2D(0.f, 180.f), Dot);

			// If the mapped dot product value less than or equal to the max angle HitActorOwner can face from the projectile
			// then HitActorMagicChargeComp can accept magic charge
			return MapDot <= FMath::Abs(HitActorMagicChargeComp.GetMaxOwnerFaceAwayFromChargeSource());
		}
	}

	return true;
}


void ASMagicProjectileBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASMagicProjectileBase, InitialSpeed);
	DOREPLIFETIME(ASMagicProjectileBase, Mass);
	DOREPLIFETIME(ASMagicProjectileBase, Drag);
	DOREPLIFETIME(ASMagicProjectileBase, Gravity);

}


