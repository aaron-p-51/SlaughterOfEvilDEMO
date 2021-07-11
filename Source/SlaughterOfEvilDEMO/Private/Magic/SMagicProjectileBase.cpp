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

// Sets default values
ASMagicProjectileBase::ASMagicProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	if (SphereComp)
	{
		SetRootComponent(SphereComp);
		SphereTraceRadius = SphereComp->GetScaledSphereRadius();
	}

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	if (SphereComp)
	{
		MeshComp->SetupAttachment(GetRootComponent());
	}


	// Set defaults
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

	SetReplicates(true);

}


// Called when the game starts or when spawned
void ASMagicProjectileBase::BeginPlay()
{
	Super::BeginPlay();

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

	CalculateMovement(DeltaTime);

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
		
		/*if (OnHitEffects)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OnHitEffects, HitResult.Location, UKismetMathLibrary::MakeRotFromX(HitResult.Normal));
		}*/

		if (GetLocalRole() == ENetRole::ROLE_Authority)
		{
			TryApplyMagicCharge(HitResults);
		}
		
	}


	return bHitDetected;
}


bool ASMagicProjectileBase::TryApplyMagicCharge(TArray<FHitResult>& HitResult)
{
	for (auto HitResult : HitResult)
	{
		auto MagicChargableActor = Cast<ASMeleeWeaponBase>(HitResult.GetActor());
		if (MagicChargableActor)
		{
			if (MagicChargableActor->GetMeleeWeaponState() == EMeleeWeaponState::EMWS_Blocking)
			{
				MagicChargableActor->TrySetMagicCharge(true);
			}
		}
	}

	//if (ActorToMagicCharge)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *ActorToMagicCharge->GetName());

	//	auto MeleeWeaponWielderInterface = Cast<ISMeleeWeaponWielder>(ActorToMagicCharge);
	//	if (MeleeWeaponWielderInterface)
	//	{
	//		if (MeleeWeaponWielderInterface->IsBlocking())
	//		{
	//			float x = FVector::DotProduct(GetActorForwardVector(), ActorToMagicCharge->GetActorForwardVector());
	//			
	//			// TODO: Scale MaxBlockAngle of Dot product results
	//			if (x >= -1.f && x <= -0.8f)
	//			{
	//				return MeleeWeaponWielderInterface->TrySetMagicCharge(true);
	//			}
	//		}
	//	}
	//}

	return false;
}


void ASMagicProjectileBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASMagicProjectileBase, InitialSpeed);
	DOREPLIFETIME(ASMagicProjectileBase, Mass);
	DOREPLIFETIME(ASMagicProjectileBase, Drag);
	DOREPLIFETIME(ASMagicProjectileBase, Gravity);

}


