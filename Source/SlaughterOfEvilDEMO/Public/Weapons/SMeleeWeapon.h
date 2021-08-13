// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/SWeaponBase.h"
#include "SMeleeWeapon.generated.h"


class USoundCue;
class USphereComponent;
class ASMagicProjectileBase;

UENUM(BlueprintType)
enum class EMeleeWeaponUseState : uint8
{
	EMWUS_Idle			UMETA(DisplayName = "Idle"),
	EMWUS_Attacking		UMETA(DisplayName = "Attacking"),
	EMWUS_Blocking		UMETA(DisplayName = "Blocking")
};


UENUM(BlueprintType)
enum class EMeleeWeaponChargeState : uint8
{
	EMWCS_Idle			UMETA(DisplayName = "Idle"),
	EMWCS_Charging		UMETA(DisplayName = "Charging"),
	EMWCS_Charged		UMETA(DisplayName = "Charged"),
	EMWCS_Finish		UMETA(DisplayName = "Finish")
};


/**
 * Charge attacks are attacks where attack is started and held until released
 * at a later point in time. There will be a looping animation needed
 */
USTRUCT(BlueprintType)
struct FMeleeWeaponChargeAnims
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	FUseWeaponAnims Start;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	FUseWeaponAnims Finish;
};

/**
 * Melee Weapon data:
 *		A FPP and TPP weapon will exist for each player. Depending of the players perspective only the proper weapon will be shown
 *		All game play essential collisions will only be calculate from the FPPMeleeWeapon on the server.
 */
USTRUCT(BlueprintType)
struct FMeleeWeaponData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float ApplyDamageForwardOffset = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float ApplyDamageRadius = 50.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	bool bDrawDebugDamageSphere;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float BaseDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(EditDefaultsOnly, Category = "Animation | Attacks")
	TArray<FUseWeaponAnims> Attacks;

	UPROPERTY(EditDefaultsOnly, Category = "Animation | ChargeAttacks")
	TArray<FMeleeWeaponChargeAnims> ChargeAttacks;

	UPROPERTY(EditDefaultsOnly, Category = "Animation | MagicChargeAttacks")
	FMeleeWeaponChargeAnims MagicChargeAttack;

	UPROPERTY(EditDefaultsOnly, Category = "Animation | Blocking")
	FUseWeaponAnims BlockImpactAnims;

	UPROPERTY(EditDefaultsOnly, Category = "Animation | Blocking")
	FUseWeaponAnims StartBlockAnims;

	UPROPERTY(EditDefaultsOnly, Category = "Animation | Blocking")
	FUseWeaponAnims StopBlockAnims;

	UPROPERTY(EditDefaultsOnly)
	bool bCanMagicBlock;
};


USTRUCT(BlueprintType)
struct FCosmeticCollisionPoints
{
	GENERATED_BODY()

	UPROPERTY()
	FVector PreviousLocation;

	UPROPERTY()
	FVector CurrentLocation;
};

/**
 * 
 */
UCLASS()
class SLAUGHTEROFEVILDEMO_API ASMeleeWeapon : public ASWeaponBase
{
	GENERATED_BODY()


/**
 * Members
 */

protected:

	/*************************************************************************/
	/* Weapon Data */
	/*************************************************************************/

	UPROPERTY(EditDefaultsOnly)
	FMeleeWeaponData MeleeWeaponData;

	UPROPERTY(ReplicatedUsing = OnRep_MeleeAttackCount)
	uint8 MeleeAttackCount = 0;

	UPROPERTY(EditDefaultsOnly)
	USceneComponent* CosmeticCollisionPoint;

	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* FleshCollisionVFX;

	UPROPERTY(EditDefaultsOnly)
	USoundCue* FleshCollisionSFX;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	EMeleeWeaponUseState UseState;

	UPROPERTY(ReplicatedUsing = OnRep_ChargeState, VisibleAnywhere, BlueprintReadOnly)
	EMeleeWeaponChargeState ChargeState;

	/** Collision Comp  */
	// UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	// UCapsuleComponent* CosmeticCollisionComp;

	UPROPERTY(EditDefaultsOnly)
	USphereComponent* MagicChargeCollisionComp;


	uint8 bCosmeticTraceEnable : 1;

	/** Actors to ignore when doing weapon trace when MeleeWeaponState is EMWS_Attacking  */
	UPROPERTY()
	TArray<AActor*> CosmeticTraceIgnoreActors;

	/** Position of CollisionComp last frame */
	FVector PreviousCosmeticTraceCenter;

	float TraceRadius;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ASMagicProjectileBase> DefaultMagicProjectile;

	UPROPERTY(ReplicatedUsing = OnRep_IsBlocking)
	uint8 bIsBlocking : 1;


/**
 * Methods
 */
public:
	
	ASMeleeWeapon();

	/** Called every frame */
	virtual void Tick(float DeltaTime) override;

protected:

	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;


	/*************************************************************************/
	/* Melee Attack (Use) */
	/*************************************************************************/
public:

	/** [Server + Client] Use (Attack) with this weapon */
	virtual void Use() override;
	
	/** [Server + Client] Called from anim notify when attack is finished, prevents from attacking too fast */
	UFUNCTION()
	void OnUseFinish();

protected:

	/** Calles Use() on server */
	UFUNCTION(Server, Reliable)
	void ServerUse();

	/** [Server + Client] Increment index of @MeleeWeaponData.Attacks. */
	UFUNCTION()
	void IncrementMeleeAttackCount();

	/** [Client] Trigger animations for simulated proxies,  COND_SkipOwner is not working hence IsLocallyControlled check*/
	UFUNCTION()
	void OnRep_MeleeAttackCount();


	/*************************************************************************/
	/* Melee Block */
	/*************************************************************************/
public:

	/** [Server + Client] transition weapon to block state. If MeleeWeaponData.bCanMagicBlock is true will enable @MagicChargeCollisionComp */
	void StartBlock();

	/** [Server + Client] transition weapon out of block state. If MeleeWeaponData.bCanMagicBlock is true will disable @MagicChargeCollisionComp */
	void StopBlock();

	/** [Server + Client] Is @UseState set to EMWUS_Blocking */
	FORCEINLINE bool GetIsBlocking() const { return bIsBlocking; }

protected:

	/** Calls StartBlock on server */
	UFUNCTION(Server, Reliable)
	void ServerStartBlock();

	/** Calls StopBlock on server */
	UFUNCTION(Server, Reliable)
	void ServerStopBlock();

	/** [Client] Trigger animations for simulated proxies,  COND_SkipOwner is not working hence IsLocallyControlled check*/
	UFUNCTION()
	void OnRep_IsBlocking();

	/*************************************************************************/
	/* Magic Charge */
	/*************************************************************************/
public:

	/** [Server + Client]  If @bisMagicCharged is true start to use magic charge attack*/
	virtual void StartUseMagicCharge() override;

	/** [Server + Client] If @bisMagicCharged and ChargeState is set to Charged, finish magic charge attack, release magic charge*/
	virtual void FinishUseMagicCharge() override;

	/** [Server + Client] is ChargeState == EMWCS_Charged */
	UFUNCTION()
	bool GetIsWeaponMagicChargeReady() const;

	/*[Server + Client] Enable Query Collisions for MagicChargeCollisionComp*/
	UFUNCTION()
	void EnableMagicChargeCollisions(bool Enable);

	/** [Server] Called from anim notify when animation reaches a point signifying attack is charged (ie max height of swing) */
	UFUNCTION()
	virtual void OnMagicChargeReady();

	/** [Server] Called from anim notify when to release magic charge */
	virtual void OnMagicChargeRelease() override;

	

protected:

	/** Calls StartUseMagicCharge on server */
	UFUNCTION(Server, Reliable)
	void ServerStartUseMagicCharge();

	/** Calls FinishUseMagicCharge on server */
	UFUNCTION(Server, Reliable)
	void ServerFinishMagicCharge();

	/** [Client] Trigger animations for simulated proxies, COND_SkipOwner is not working hence IsLocallyControlled check
	 *  for starting and finishing using magic charge */ 
	UFUNCTION()
	void OnRep_ChargeState();

	/** [Server] Called when attached MagicChargeComponent changes between charged and not charged, or vise versa */
	virtual void OnMagicChargeStateChange(bool IsMagicCharged) override;

	/**
	 * [Server] Release Magic Charge from weapon
	 * 
	 * @param	Transform to spawn released magic charge
	 */
	UFUNCTION()
	virtual void ReleaseMagicCharge(FTransform ReleaseTransform) PURE_VIRTUAL(ASMeleeWeapon::ReleaseMagicCharge, );

	/*************************************************************************/
	/* Melee Weapon State */
	/*************************************************************************/
public:

	/** [Server + Client] return true if weapon is currently idle state */
	bool WeaponIsIdle() const;

	/** [Server + Client] return true if weapon is currently in attack state */
	bool WeaponIsAttacking() const;

	/** [Server + Client] return true if weapon is currently in block state */
	bool WeaponIsBlocking() const;

private:

	/**
	 * [Server] Try and set the UseState of the weapon
	 * 
	 * @param	New use state to transition to.
	 * @return	UseState was changed successfully 
	 */
	UFUNCTION()
	virtual bool TrySetUseState(EMeleeWeaponUseState NewMeleeWeaponUseState);

	/**
	 * [Server] Try and set the weapon ChargeState, magic charge status does not effect transitioning
	 * through charge states
	 * 
	 * @param	New ChargeState to transition to
	 * @return	ChargeState change success
	 */
	UFUNCTION()
	virtual bool TrySetChargeState(EMeleeWeaponChargeState NewMeleeWeaponChargeState);

	/*************************************************************************/
	/* Apply Damage */
	/*************************************************************************/
public:

	/**
	 * [Server] Try and apply damage to actors from weapon attacking. Actors are detected from overlapping sphere
	 * when TryApplyDamamge is called. See @SAnimNotifyApplyDamage, notify is placed when weapon is at middle of attacking
	 * point in swing.
	 */
	void TryApplyDamage();

	/*************************************************************************/
	/* FX */
	/*************************************************************************/

public:

	/** [Server + Client] Enable Cosmetic FX when weapon collides with target. All collisions detected for FX happen locally */
	void EnableCosmeticWeaponTrace(bool Enable);

protected:

	/**
	 * [Server + Client] Check for a hit detected when weapon is attacking
	 * 
	 * @param first hit detected
	 */
	virtual bool CosmeticCollisionTrace(FHitResult& HitResult);
	
	/** [Server + Client] Display cosmetic VFX from @CosmeticCollisionTrace */
	UFUNCTION()
	void DisplayCosmeticVFX();


};
