// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SMagicProjectileBase.generated.h"


class UParticleSystem;
class USphereComponent;
class UMeshComponent;
class USMagicChargeComponent;
class UProjectileMovementComponent;

UCLASS()
class SLAUGHTEROFEVILDEMO_API ASMagicProjectileBase : public AActor
{
	GENERATED_BODY()
	

/**
 * Members
 */

private:

	/*****************************************************************/
	/* Status */
	/*****************************************************************/

	/** CurrentPosition this frame, used to detect collisions */
	UPROPERTY()
	FVector CurrentPosition;

	/** Position last frame, used to detect collisions */
	UPROPERTY()
	FVector PreviousPosition;

	/** Radius to detect collision hit, will be radius of SphereComp */
	UPROPERTY()
	float SphereTraceRadius;

	/** If the projectile is homing then the projectile will track this actor. This should not be set directly see @SetHomingTargetActor  */
	UPROPERTY(ReplicatedUsing = OnRep_HomingTargetActor)
	AActor* HomingActor;

	/** Cached distance, prevent Sqrt() see @StopHomingDistance */
	float StopHomingDistanceSquared;

	/** Has the projectile got within StopHomingDistance */
	bool bHasEnteredStopHomingDistance;



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

	/** Projectile Movement. Movement is not replicated all clients will simulate on their owne */
	UPROPERTY(EditDefaultsOnly)
	UProjectileMovementComponent* ProjectileMovement;

	/*****************************************************************/
	/* Gameplay */
	/*****************************************************************/

	/** Base damage this projectile will cause upon impact */
	UPROPERTY(EditAnywhere, Category = "Gameplay")
	float BaseDamage;

	/**
	 * Is it possible for another actor to block this projectile. Blocking could be not taking
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

	/** Life Time of the projectile, after LifeTime is exceeded from time of spawn, this actor will be destroyed */
	UPROPERTY(EditDefaultsOnly)
	float LifeTime;

	/** Play the OnHitEffects if upon destruction if LifeTime is exceeded */
	UPROPERTY(EditDefaultsOnly)
	bool bPlayOnHitEffectsLifeTimeExceeded;

	/** TimerHandle to keep track of LifeTime */
	FTimerHandle TimerHandle_LifeTime;

	/** Once the projectile get with in this distance and then exits the projectile will stop homing, prevents the projectile
	 * from hitting the Player when player is not facing projectile*/
	UPROPERTY(EditDefaultsOnly)
	float StopHomingDistance;

 /**
  * Methods
  */


public:	

	/** Sets default values for this actor's properties */
	ASMagicProjectileBase();

	/** Called every frame */
	virtual void Tick(float DeltaTime) override;

	/** [Server] Set the homing targets, will only have effect if projectile movement component has homing enabled,
	 * and passed in TargetActor. Logic to setup homing component see @OnRep_HomingTargetActor()
	 * 
	 * @param TargetActor		Actor for ProjectileMovement to track;
	 */
	UFUNCTION()
	void SetHomingTargetActor(AActor* TargetActor);


protected:

	/**  Called when the game starts or when spawned */
	virtual void BeginPlay() override;

private:

	/**
	 * [Server + Client] Detect if hit will occur when projectile moves from PreviousPosition to NextPosition.
	 * 
	 * @param HitResults	HitResults will be populated with collisions detected
	 * return	ture if at least one actor was detected
	 */
	bool DetectHit(TArray<FHitResult>& HitResults) const;

	/**
	 * [Server + Client] Emit OnHitEffects
	 * @param HitResults	Results from trace used to detect hit
	 *
	 */
	void EmitOnHitEffects(const TArray<FHitResult>& HitResults) const;

	/**
	 * [Server] If hit is detected try and apply magic charge to actor if actor has USMagicChargeComponent
	 * @params HitResult	All actors that were detect from DetectHit
	 * 
	 * return true if magic charge was applied
	 */
	bool TryApplyMagicCharge(const TArray<FHitResult>& HitResults) const;

	/**
	 * [Client + Server] Get the closest Actors HitResult to this actors position. HitResults generated in trace done by DetectHit()
	 * @param HitResults	All actors hit in last call to DetectHit
	 * 
	 * return HitResult of closest Actor Hit
	 */
	FHitResult GetFirstHitActorHitResult(const TArray<FHitResult>& HitResults) const;

	/**
	 * [Server + Client] Called when HomingActor is set via SetHomingTargetActor called on server.
	 * Will set ProjectileMovement->HomingTargetComponent. The actor passed in to SetHomingTargetActor
	 * needs to implement ISProjectileMagicTarget for the HomingTargetComponent to be properly set up
	 */
	UFUNCTION()
	void OnRep_HomingTargetActor();

	/** [Server + Client] Called when LifeTime is exceeded. Will destroy actor */
	UFUNCTION()
	void LifeTimeExceeded();

	/**
	 * [Server + Client] Check the distance to between this actor and HomingActor. Once the distance between the two actors is less than
	 * StopHomingDistance bHasEnteredStopHomingDistance will be set. Once this actor then gets farther away than StopHomingDistance it will stop homing
	 * this is to prevent homing projectile to circle back and hit target (Player) from behind 
	 
			TODO: This will not work. If projectile moves past target in single frame bHasEnteredStopHomingDistance will never be set.
			build vector to intercest with target (maybe) try again later
	 */
	void StopHomingDistanceCheckAndApply();

};
