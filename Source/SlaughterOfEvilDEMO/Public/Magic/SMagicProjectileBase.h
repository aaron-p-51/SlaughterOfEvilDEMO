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
	/* Gameplay */
	/*****************************************************************/

	/** Base damage this projectile will cause upon impact */
	UPROPERTY(EditAnywhere, Category = "Gameplay")
	float BaseDamage;

	/**
	 * Is it possible for anoter actor to block this projectile. Blocking could be not taking
	 * damage or magic charging
	 */
	UPROPERTY(EditAnywhere, Category = "Gameplay")
	uint32 bCanBeBlocked : 1;

	/** Upon impact that is not blocked will this projectile cause damage */
	UPROPERTY(EditAnywhere, Category = "Gameplay")
	uint32 bCausesDamage : 1;

	/** Effects to emit upon collision */
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* OnHitEffects;

	/** Collision Channel used to detect hits */
	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<ECollisionChannel> CollisionChannel;

	/*****************************************************************/
	/* Status */
	/*****************************************************************/

	/** Current acceleration this frame */
	UPROPERTY(BlueprintReadOnly)
	FVector Acceleration;

	/** Current velocity this frame */
	UPROPERTY(BlueprintReadOnly)
	FVector Velocity;

	/** Position projectile should move to this frame */
	UPROPERTY(BlueprintReadOnly)
	FVector NextPosition;

	/** Position of projectile last frame */
	UPROPERTY(BlueprintReadOnly)
	FVector PreviousPosition;

	/** Radius to detect hit, will be radius of SphereComp */
	UPROPERTY(BlueprintReadOnly)
	float SphereTraceRadius;

public:

	/*****************************************************************/
	/* Movement Configuration */
	/*****************************************************************/

	/** Initial speed of projectile when spawned, will not have any effect if changed after spawning */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Config|Projectile Properties", meta = (ExposeOnSpawn = "true"))
	float InitialSpeed;

	/** Mass of projectile, can be changed during life of projectile */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Config|Projectile Properties", meta = (ExposeOnSpawn = "true"))
	float Mass;

	/** Drag of projectile (N), can be changed during life of projectile */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Config|Projectile Properties", meta = (ExposeOnSpawn = "true"))
	float Drag;

	/** Effect of gravity on projectile, can be changed during life of projectile */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Config|Projectile Properties", meta = (ExposeOnSpawn = "true"))
	FVector Gravity;

 /**
  * Methods
  */


public:	

	/**  Sets default values for this actor's properties */
	ASMagicProjectileBase();

	// Called every frame
	virtual void Tick(float DeltaTime) override;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	/**
	 * [Server + Client] Calculate projectile movement this frame, movement is not replicated, all clients
	 * will move the projectile themselves
	 * 
	 * @param DeltaTime			Time between frames
	 */
	void CalculateMovement(float DeltaTime);

	/**
	 * [Server + Client] Detect if hit will occur when projectile moves from PreviousPosition to NextPosition.
	 * 
	 * return	ture if at least one actor was detected
	 */
	bool DetectHit();

	/**
	 * [Server + Client] Emit OnHitEffects
	 * @param HitResults	Results from trace used to detect hit
	 *
	 */
	void EmitOnHitEffects(TArray<FHitResult>& HitResults) const;

	/**
	 * [Server] If hit is detected try and apply magic charge to actor if actor has USMagicChargeComponent
	 * @params HitResult	All actors that were detect from DetectHit
	 * 
	 * return true if magic charge was applied
	 */
	bool TryApplyMagicCharge(TArray<FHitResult>& HitResults) const;

	/**
	 * [Client + Server] Get the closest Actors HitResult to this actors position. HitResults generated in trace done by DetectHit()
	 * @param HitResults	All actors hit in last call to DetectHit
	 * 
	 * return HitResult of closest Actor Hit
	 */
	FHitResult GetFirstHitActorHitResult(TArray<FHitResult>& HitResults) const;


};
