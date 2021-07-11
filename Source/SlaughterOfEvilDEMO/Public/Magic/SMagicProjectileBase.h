// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SMagicProjectileBase.generated.h"


class UParticleSystem;
class USphereComponent;
class UMeshComponent;

UCLASS()
class SLAUGHTEROFEVILDEMO_API ASMagicProjectileBase : public AActor
{
	GENERATED_BODY()
	

/**
 * Members
 */

protected:

	UPROPERTY(VisibleAnywhere)
	USphereComponent* SphereComp;

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

	UPROPERTY(EditAnywhere, Category = "Gameplay")
	float MaxBlockAngle;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* OnHitEffects;

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

	UPROPERTY(BlueprintReadOnly)
	FVector NextPosition;

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

	void CalculateMovement(float DeltaTime);

	bool DetectHit();

	bool TryApplyMagicCharge(TArray<FHitResult>& HitResult);


};
