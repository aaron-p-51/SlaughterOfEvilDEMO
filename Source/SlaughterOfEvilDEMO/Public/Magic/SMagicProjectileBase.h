// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SMagicProjectileBase.generated.h"


class UParticleSystem;
class USphereComponent;
class UMeshComponent;
class USMagicChargeComponent;

UCLASS()
class SLAUGHTEROFEVILDEMO_API ASMagicProjectileBase : public AActor
{
	GENERATED_BODY()
	

/**
 * Members
 */

protected:

	/*************************************************************************/
	/* Components*/
	/*************************************************************************/

	/** Collisions */
	UPROPERTY(VisibleAnywhere)
	USphereComponent* SphereComp;

	/** Can be null, used really for debug purposes */
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshComp;

	/*****************************************************************/
	/* Movement Configuration */
	/*****************************************************************/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Config|Projectile Properties", meta = (ExposeOnSpawn="true"))
	float InitialSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Config|Projectile Properties", meta = (ExposeOnSpawn = "true"))
	float Mass;

	/** Drag of projectile (N) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Config|Projectile Properties", meta = (ExposeOnSpawn = "true"))
	float Drag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Config|Projectile Properties", meta = (ExposeOnSpawn = "true"))
	FVector Gravity;

	/*****************************************************************/
	/* Gameplay */
	/*****************************************************************/

	UPROPERTY(EditAnywhere, Category = "Gameplay")
	float BaseDamage;

	UPROPERTY(EditAnywhere, Category = "Gameplay")
	uint32 bCanBeBlocked : 1;

	UPROPERTY(EditAnywhere, Category = "Gameplay")
	uint32 bCausesDamage : 1;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* OnHitEffects;

	/** Collision Channel used to detect hits */
	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<ECollisionChannel> CollisionChannel;

protected:

	/*****************************************************************/
	/* Status */
	/*****************************************************************/

	UPROPERTY(BlueprintReadOnly)
	FVector Acceleration;

	UPROPERTY(BlueprintReadOnly)
	FVector Velocity;

	/** Position projectile should move to this frame */
	UPROPERTY(BlueprintReadOnly)
	FVector NextPosition;

	/** Position of projectile last frame */
	UPROPERTY(BlueprintReadOnly)
	FVector PreviousPosition;

	/** Effect drag has on mass Drag/Mass (N/m) */
	UPROPERTY(BlueprintReadOnly)
	float DragEffect;

	/** Radius to detect hit, will be radius of SphereComp */
	UPROPERTY(BlueprintReadOnly)
	float SphereTraceRadius;

 /**
  * Methods
  */


public:	
	// Sets default values for this actor's properties
	ASMagicProjectileBase();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	/**
	 * Calculate projectile movement this frame
	 * @param DeltaTime			Time between frames
	 */
	void CalculateMovement(float DeltaTime);

	/**
	 * Detect if hit will occur when projectile moves. A hit will be detected from a sphere trace 
	 * between PreviousPosition and NextPosition
	 * 
	 * return	ture if at least one actor was detected
	 */
	bool DetectHit();

	/**
	 * TODO: if hit multiple actors (player and longsword) if longsword has successful magic charge set do not apply damage to player
	 * 
	 * [Server] If hit is detected try and apply magic charge to actor if actor has USMagicChargeComponent
	 * @params HitResult	All actors that were detect from DetectHit
	 * 
	 * return true if magic charge was applied
	 */
	bool TryApplyMagicCharge(TArray<FHitResult>& HitResults);

	/**
	 * [Server] Verify the hit actor can take magic charge based on parameters set in MagicChargeComp
	 * @param HitActors MagicChargeComponent
	 * 
	 * return Actor can accept magic charge
	 */
	bool HitActorCanAcceptMagicCharge(USMagicChargeComponent& HitActorMagicChargeComp);

	/**
	 * [Client + Server] Get the closest Actors HitResult to this actors position. HitResults generated in trace done by DetectHit()
	 * @param HitResults	All actors hit in last call to DetectHit
	 * 
	 * return HitResult of closest Actor Hit
	 */
	FHitResult GetFirstHitActorHitResult(TArray<FHitResult>& HitResults);


};
