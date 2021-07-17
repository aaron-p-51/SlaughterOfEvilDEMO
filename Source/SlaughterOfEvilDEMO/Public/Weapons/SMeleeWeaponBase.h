// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SMeleeWeaponBase.generated.h"

class UCapsuleComponent;
class USphereComponent;
class USMagicChargeComponent;

UENUM(BlueprintType)
enum class EMeleeWeaponState : uint8
{
	EMWS_Idle			UMETA(DisplayName="Idle"),
	EMWS_Attacking		UMETA(DisplayName="Attacking"),
	EMWS_Blocking		UMETA(DisplayName="Blocking")
};


UCLASS(Abstract, Blueprintable)
class SLAUGHTEROFEVILDEMO_API ASMeleeWeaponBase : public AActor
{
	GENERATED_BODY()
	
/**
 * Members
 */

public:


protected:

	/*************************************************************************/
	/* Components*/
	/*************************************************************************/

	/** RootComp so MeshComp can be offset */
	UPROPERTY(VisibleDefaultsOnly)
	USceneComponent* RootComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UStaticMeshComponent* MeshComp;

	/** Collision Comp  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UCapsuleComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USMagicChargeComponent* MagicChargeComp;

	/*************************************************************************/
	/* State Variables */
	/*************************************************************************/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EMeleeWeaponState MeleeWeaponState;

	/*************************************************************************/
	/* Damage */
	/*************************************************************************/
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	bool bDrawDebugHitSphere;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float BaseDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	uint32 bCanDamageMultipleTargetsPerUse : 1;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<UDamageType> DamageTypeClass;

	/*************************************************************************/
	/* Damage Calculations */
	/*************************************************************************/

	/** Position of CollisionComp last frame */
	FVector PreviousCollisionCenter;

	// Cached Trace arguments for capsual trace
	float TraceRadius;
	float TraceHalfHeight;

	/** Actors to ignore when doing weapon trace when MeleeWeaponState is EMWS_Attacking  */
	UPROPERTY()
	TArray<AActor*> DamageTraceIgnoreActors;

	/** Object types to detect when doing weapon trace when MeleeWeaponState is EMWS_Attacking */
	UPROPERTY()
	TArray<TEnumAsByte<EObjectTypeQuery>> DamageCollisionObjectTypes;


	FHitResult HitResult;


	/*************************************************************************/
	/* Gameplay */
	/*************************************************************************/

	/**
	 * Separate MeleeWeapons for TPP and FPP will be spawned. Visibility will be set based on need to show
	 * for example if FPP then TPP weapon will not be visible
	 */
	uint32 bWeaponVisibility : 1;


 /**
  * Methods
  */

public:	
	
	/** Sets default values for this actor's properties */
	ASMeleeWeaponBase(); 

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/*************************************************************************/
	/* Magic  */
	/*************************************************************************/

	/**
	 * [Server + Client] Apply effects to indicate magic charge state, effects should part of
	 * child class
	 */
	UFUNCTION()
	virtual void ApplyMagicChargeEffects() PURE_VIRTUAL(ASMeleeWeaponBase::ApplyMagicChargeEffects, );

	/**
	 * [Server + Client] Remove effects to indicate magic charge state, effects should part of
	 * child class
	 */
	UFUNCTION()
	virtual void RemoveMagicChargeEffects() PURE_VIRTUAL(ASMeleeWeaponBase::RemoveMagicChargeEffects, );

	UFUNCTION()
	virtual void ReleaseMagicCharge(FTransform& ReleaseTransform) PURE_VIRTUAL(ASMeleeWeaponBase::ReleaseMagicCharge, );


	/*************************************************************************/
	/* Accessors */
	/*************************************************************************/

	/**
	 * [Server + Client] Get current MeleeWeaponState
	 */
	UFUNCTION()
	virtual EMeleeWeaponState GetMeleeWeaponState() const;

	/**
	 * [Server] Get if this weapon is magic charged
	 */
	UFUNCTION()
	bool GetIsWeaponMagicCharged();

	UFUNCTION()
	bool TEMP_DELETE_ME_ApplyMagic();



	/*************************************************************************/
	/* Gameplay */
	/*************************************************************************/
	
	/**
	 * [Server + Client] Set Weapon can cause damage, will change MeleeWeaponState to EMWS_Attacking
	 * if CanDamage is set to false will set MeleeWeaponState to EMWS_Idle.
	 * 
	 * @param CanDamage		Should Melee Weapon apply damage upon CollisionComp trace hit detected
	 * 
	 * return success if MeleeWeapon changed to EMWS_Idle when CanDamage is false, or EMWS_Attacking when CanDamage is true 
	 */
	virtual bool SetCanCauseDamage(bool CanDamage);

	/**
	* [Server + Client] Set Weapon can cause damage, will change MeleeWeaponState to EMWS_Blocking
	* if Blocking is set to false will set MeleeWeaponState to EMWS_Idle.
	*
	* @param Blocking		Should Melee Weapon enable CollisionComp to query to detect collisions
	*
	* return success if MeleeWeapon changed to EMWS_Idle when CanDamage is Blocking, or EMWS_Blocking when Blocking is true
	*/
	virtual bool SetIsBlocking(bool Blocking);

	/**
	 * [Server + Client] Set Weapon Visibility, bWeaponVisibility
	 */
	FORCEINLINE void SetWeaponVisibility(bool Value) { bWeaponVisibility = Value; }


protected:

	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;


private:

	/*************************************************************************/
	/* Compute and apply damage */
	/*************************************************************************/

	/**
	 * [Server] Check for a hit detected when weapon is attacking
	 * return was a hit detected
	 */
	virtual bool CheckForAttackTraceCollision();

	/**
	 * [Server] Cache data need to perform trace in CheckForAttackTraceCollision
	 */
	void CacheDamageTraceArguments();

	/*************************************************************************/
	/* Magic */
	/*************************************************************************/
	
	/**
	 * [Server] Bound to MagicChargeComp delegate. Is called with magic charge state changes in MagicChargeComp
	 */
	UFUNCTION()
	void OnMagicChargeChange(bool Charged);

	/**
	* [Server + Client] Try and Set Melee Weapon State.
	* @param NewMeleeWeaponState		Melee Weapon state to change to
	*
	* return success of setting NewMeleeWeaponState
	*/
	UFUNCTION()
	virtual bool TrySetMeleeWeaponState(EMeleeWeaponState NewMeleeWeaponState);

};
