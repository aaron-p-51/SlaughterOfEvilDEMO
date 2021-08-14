 //Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SMeleeWeaponWielder.h"
#include "SProjectileMagicCaster.h"
#include "SProjectileMagicTarget.h"
#include "SCharacterBase.generated.h"



class UCameraComponent;
class ASWeaponBase;
class ASMeleeWeapon;
class ASMagicProjectileBase;
class USHealthComponent;



//USTRUCT(BlueprintType)
//struct FProjectileMagicCastData
//{
//	GENERATED_BODY()
//
//	UPROPERTY(EditDefaultsOnly)
//	UAnimMontage* TPPMagicCastAnimMontage;
//
//	UPROPERTY(EditDefaultsOnly)
//	UAnimMontage* FPPMagicCastAnimMontage;
//
//	/** Only used for AI, min distance from target to use this MeleeAttack  */
//	UPROPERTY(EditDefaultsOnly)
//	float MinDistance;
//
//	/** Only used for AI, max distance from target to use this MeleeAttack  */
//	UPROPERTY(EditDefaultsOnly)
//	float MaxDistance;
//
//	UPROPERTY(EditDefaultsOnly)
//	TSubclassOf<ASMagicProjectileBase> MagicProjectile;
//
//	UPROPERTY(EditDefaultsOnly)
//	FName SpawnSocket;
//
//	UPROPERTY(EditDefaultsOnly)
//	float ProjectileLaunchSpeed;
//};


UCLASS()
class SLAUGHTEROFEVILDEMO_API ASCharacterBase : public ACharacter, /*public ISMeleeWeaponWielder,*/ /*public ISProjectileMagicCaster,*/ public ISProjectileMagicTarget
{
	GENERATED_BODY()


/**
 * Members
 */
protected:

	/*************************************************************************/
	/* Components*/
	/*************************************************************************/

	UPROPERTY(EditDefaultsOnly)
	USkeletalMeshComponent* FirstPersonMesh;

	UPROPERTY(EditDefaultsOnly)
	UCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly)
	USHealthComponent* HealthComponent;
	
	/*************************************************************************/
	/* Weapon Inventory*/
	/*************************************************************************/

	/** Inventory of weapon classes assigned to this character, these classes will be spawned as default inventory */
	UPROPERTY(EditDefaultsOnly, Category = "Configuration | Weapons")
	TArray<TSubclassOf<ASWeaponBase>> WeaponInventoryClasses;

	/** skeletal mesh socket name to assign weapon mesh to. Assign same name to first and third person meshes */
	UPROPERTY(EditDefaultsOnly, Category = "Configuration | Weapons")
	FName WeaponSocket;

	/** Weapons currently in character inventory */
	UPROPERTY(Transient, Replicated)
	TArray<ASWeaponBase*> WeaponInventory;

	/** Currently equipped weapon */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_CurrentWeapon)
	ASWeaponBase* CurrentWeapon;


	/*************************************************************************/
	/* Magic Attacks */
	/*************************************************************************/

	/** Projectile magic attacks homing target */
	UPROPERTY()
	USceneComponent* ProjectileHomingTarget;


	/*************************************************************************/
	/* Animation */
	/*************************************************************************/

	/** Cached pointer to FFP mesh animation instance */
	UPROPERTY(VisibleDefaultsOnly, Category = "Configuration | Animation")
	UAnimInstance* FirstPersonAnimInstance;

	/** Cached pointer to TFP mesh animation instance */
	UPROPERTY(VisibleDefaultsOnly, Category = "Configuration | Animation")
	UAnimInstance* ThirdPersonAnimInstance;



 /**
  * Methods
  */
public:
	// Sets default values for this character's properties
	ASCharacterBase();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Get the character first person mesh */
	FORCEINLINE USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	/** Get the character third person mesh */
	FORCEINLINE USkeletalMeshComponent* GetThirdPersonMesh() const { return GetMesh(); }

	/**  */
	FORCEINLINE FName GetWeaponSocketName() const { return WeaponSocket; }

	/** Returns true if local player controller */
	bool IsFirstPerson() const;

	ASMeleeWeapon* GetCurrentMeleeWeapon();

	/**
	 * [Server + Client] Get the pich of the locally controlled character
	 */
	UFUNCTION(BlueprintCallable)
	FRotator GetRemotePitchView() const;

	/*************************************************************************/
	/* Player Input*/
	/*************************************************************************/

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Player movement input */
	UFUNCTION()
	void MoveForward(float Value);
	UFUNCTION()
	void MoveRight(float Value);

	/** Melee attack with current melee weapon */
	UFUNCTION(BlueprintCallable)
	void MeleeAttack();

	/** Get the transfrom for where to spawn magic attacks. This should be from the camera view */
	FTransform GetMagicLaunchTransform();

	/** Start blocking with the currently equipped melee weapon */
	UFUNCTION()
	void MeleeStartBlock();

	/** Stop blocking with the currently equipped melee weapon */
	UFUNCTION()
	void MeleeStopBlock();

	/** Is this character currently blocking with current melee weapon */
	UFUNCTION(BlueprintPure)
	bool IsBlocking();

	
	/** [Server + Client] Start using currently equipped weapon magic */
	UFUNCTION()
	void StartUseWeaponMagic();


	/** [Server + Client] Finish using currently equipped weapon magic  */
	UFUNCTION()
	void FinishUseWeaponMagic();

	/*************************************************************************/
	/* Animation */
	/*************************************************************************/

	/** [Server + Client] Play anim montage for local mesh */
	virtual float PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None) override;

	/**
	 * [Server] Play first person anim montage on server. Used when only first person anim has notifys that need to run on server, such as 
	 * applying damage and changing game state
	 * 
	 * @return length of AnimMontage, will be 0 if unable to play
	 */
	float PlayFirstPersonMontageOnServer(class UAnimMontage* AnimMontage);

	/** Get the amin instance, locally controlled characters will be first person anim instance, simulated proxies will be third */
	UAnimInstance* GetLocalAnimInstance();

	/*************************************************************************/
	/* Health */
	/*************************************************************************/
protected:

	UFUNCTION()
	void HealthCompOnHealthChange(USHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* HealthChangeCauser);



	virtual TWeakObjectPtr<USceneComponent> GetProjectileHomingTarget() override;



	

	UFUNCTION(BlueprintCallable)
	bool IsMeleeWeaponMagicChargeReady();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	

	USkeletalMeshComponent* GetLocalMesh();

	/*************************************************************************/
	/* Inventory */
	/*************************************************************************/

	/** [Server] Spawn inventory of melee weapons set in @MeleeWeaponInventoryClasses */
	void SpawnDefaultInventory();

	/** [Server] Add weapon to MeleeWeaponInventory */
	void AddWeapon(ASWeaponBase* Weapon);

	/** [Server] Equip weapon. Weapon must be in MeleeWeaponInventory */
	void EquipWeapon(ASWeaponBase* Weapon);

	/** Equip Weapon */
	UFUNCTION(Server, Reliable)
	void ServerEquipWeapon(ASWeaponBase* Weapon);

	/**
	 * [Server + Client] Update the current weapon
	 *
	 * @param NewWeapon			Weapon to set as CurrentWeapon
	 * @param PreviousWeapon	What will become last weapon equipped after NewWeapon is equipped
	 *							usually will be set to CurrentWeapon
	 */
	void SetCurrentWeapon(ASWeaponBase* NewWeapon, ASWeaponBase* PreviousWeapon);

	/** current weapon rep handler */
	UFUNCTION()
	void OnRep_CurrentWeapon(class ASWeaponBase* LastWeapon);


private:	

	/** Called when actor is destroyed used for clean up (deletes inventory) */
	virtual void Destroyed() override;

	/** spawn inventory, setup initial variables */
	virtual void PostInitializeComponents() override;



	/*************************************************************************/
	/* Helper Functions */
	/*************************************************************************/


	/////**
	//// * [Server] Helper function to check if current weapon in magic charged. Melee weapon is not replicated, will only be
	//// * valid on server.
	//// */
	////bool IsCurrentMeleeWeaponMagicCharged() const;

	bool IsAIControlled() const;

	///**
	// * [Server + Client] Play matching animation on autonomous and simulated proxies. Do not use to play animations with
	// * notifies for used for game play progression
	// */
	//void PlayMontagePairTPPandFPP(UAnimMontage* FPPMontage, UAnimMontage* TPPMontage) const;


	void SetProjectileHomingTargetLocation(bool IsCrouching);



};
