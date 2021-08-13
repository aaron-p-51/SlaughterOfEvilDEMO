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


//UENUM(BlueprintType)
//enum class EMeleeAttackState : uint8
//{
//	EMAS_Idle			UMETA(DisplayName = "Idle"),
//	EMAS_Attacking		UMETA(DisplayName = "Attacking"),
//	EMAS_CauseDamage	UMETA(DisplayName = "CauseDamage")
//};


////UENUM(BlueprintType)
////enum class EMagicUseState : uint8
////{
////
////	EMUS_NoCharge		UMETA(DisplayName = "NoCharge"),	// Weapon is not magic charged
////	EMUS_Idle			UMETA(DisplayName = "Idle"),		// Weapon is magic charged has not starting using magic
////	EMUS_Start			UMETA(DisplayName = "Start"),		// Weapon is magic charged, staring to use (charing)
////	EMUS_Ready			UMETA(DisplayName = "Ready"),		// Weapon is magic charged, ready to use (charged)
////	EMUS_Finish			UMETA(DisplayName = "Finish")		// Weapon is magic charged, signal release of magic
////};

//USTRUCT(BlueprintType)
//struct FMeleeAttack
//{
//	GENERATED_BODY()
//
//	UPROPERTY(EditDefaultsOnly)
//	UAnimMontage* TPPMeleeAttack;
//
//	UPROPERTY(EditDefaultsOnly)
//	UAnimMontage* FPPMeleeAttack;
//
//	/** Only used for AI, min distance from target to use this MeleeAttack  */
//	UPROPERTY(EditDefaultsOnly)
//	float MinDistance;
//
//	/** Only used for AI, max distance from target to use this MeleeAttack  */
//	UPROPERTY(EditDefaultsOnly)
//	float MaxDistance;
//};


///**
// * Melee Weapon data:
// *		A FPP and TPP weapon will exist for each player. Depending of the players perspective only the proper weapon will be shown
// *		All game play essential collisions will only be calculate from the FPPMeleeWeapon on the server.
// */
//USTRUCT(BlueprintType)
//struct FMeleeWeaponData
//{
//	GENERATED_BODY()
//
//	/** Parent class of melee weapons (ASMeleeWeaponBase) */
//	UPROPERTY(EditDefaultsOnly)
//	TSubclassOf<ASMeleeWeaponBase> MeleeWeaponClass;
//
//	/** Melee weapon for TPP, this will only be visible for simulated proxies */
//	UPROPERTY(VisibleAnywhere)
//	ASMeleeWeaponBase* TPPMeleeWeapon;
//
//	/** Melee weapon for TPP, used for all game play essentials calculations and collisions */
//	UPROPERTY(VisibleAnywhere);
//	ASMeleeWeaponBase* FPPMeleeWeapon;
//
//	/** Is this the starting weapon, if multiple are set the first weapon spawned from MeleeWeaponData will be set as starting Weapon */
//	UPROPERTY(EditDefaultsOnly)
//	bool bIsStartingWeapon;
//
//	/** Socket on mesh where to attach MeleeWeapon and FPPMeleeWeapon, Socket needs to exist on both on Mesh and FirstPersonMesh */
//	UPROPERTY(EditDefaultsOnly)
//	FName MeleeWeaponSocketName = FName(TEXT("RightHandMeleeWeaponSocket"));
//
//	UPROPERTY(EditDefaultsOnly)
//	TArray<FMeleeAttack> MeleeAttacks;
//
//	/** Animation montage for FPP when starting to use magic (charging to use) */
//	UPROPERTY(EditDefaultsOnly, Category = "Attack | First Person")
//	UAnimMontage* FPPMagicStartMontage;
//
//	/** Animation montage for FPP to use magic, done after fully charged (FPPMagicStartMontage)  */
//	UPROPERTY(EditDefaultsOnly, Category = "Attack | First Person")
//	UAnimMontage* FPPMagicFinishMontage;
//
//	/** Animation montage for TPP when starting to use magic (charging to use) */
//	UPROPERTY(EditDefaultsOnly, Category = "Attack | Third Person")
//	UAnimMontage* TPPMagicStartMontage;
//
//	/** Animation montage for TPP when starting to use magic (charging to use) */
//	UPROPERTY(EditDefaultsOnly, Category = "Attack | Third Person")
//	UAnimMontage* TPPMagicFinishMontage;
//
//	/** Impact montage when weapon is blocking, will only play on locally controlled character (FPP) */
//	UPROPERTY(EditDefaultsOnly, Category = "Block | First Person")
//	UAnimMontage* BlockImpactMontage;
//};


USTRUCT(BlueprintType)
struct FProjectileMagicCastData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* TPPMagicCastAnimMontage;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* FPPMagicCastAnimMontage;

	/** Only used for AI, min distance from target to use this MeleeAttack  */
	UPROPERTY(EditDefaultsOnly)
	float MinDistance;

	/** Only used for AI, max distance from target to use this MeleeAttack  */
	UPROPERTY(EditDefaultsOnly)
	float MaxDistance;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ASMagicProjectileBase> MagicProjectile;

	UPROPERTY(EditDefaultsOnly)
	FName SpawnSocket;

	UPROPERTY(EditDefaultsOnly)
	float ProjectileLaunchSpeed;
};


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

	/** Camera is for FPP */
	UPROPERTY(EditDefaultsOnly)
	UCameraComponent* Camera;
	
	/*************************************************************************/
	/* Weapons*/
	/*************************************************************************/

	/** Inventory of weapons assigned to this player */
	UPROPERTY(EditDefaultsOnly, Category = "Configuration | Weapons")
	TArray<TSubclassOf<ASWeaponBase>> WeaponInventoryClasses;

	/**  */
	UPROPERTY(EditDefaultsOnly, Category = "Configuration | Weapons")
	FName WeaponSocket;

	UPROPERTY(Transient, Replicated)
	TArray<ASWeaponBase*> WeaponInventory;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_CurrentWeapon)
	ASWeaponBase* CurrentWeapon;

	///** Current weapon being used from MeleeWeaponData */
	//uint32 CurrentWeaponIndex;

	/*************************************************************************/
	/* Magic Attacks */
	/*************************************************************************/

	//UPROPERTY(EditDefaultsOnly, Category = "Configuration | Projectile Magic")
	//TArray<FProjectileMagicCastData> ProjectileMagicCasts;

	/*uint32 CurrentProjectMagicCastIndex;*/

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

	/*************************************************************************/
	/* State */
	/*************************************************************************/

	/*UPROPERTY(Replicated)
	uint32 bIsBlocking : 1;*/

	////UPROPERTY(ReplicatedUsing=OnRep_CurrentWeaponIsMagicCharged)
	////uint32 bCurrentWeaponIsMagicCharged : 1;

	//UPROPERTY(Replicated)
	//EMagicUseState MagicUseState;

	//EMeleeAttackState MeleeAttackState;


private:



 /**
  * Methods
  */
public:
	// Sets default values for this character's properties
	ASCharacterBase();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	FORCEINLINE USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	FORCEINLINE USkeletalMeshComponent* GetThirdPersonMesh() const { return GetMesh(); }

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


	/** Movement */
	UFUNCTION()
	void MoveForward(float Value);
	UFUNCTION()
	void MoveRight(float Value);

	/*************************************************************************/
	/* Melee Weapon Attack*/
	/*************************************************************************/
	UFUNCTION(BlueprintCallable)
	void MeleeAttack();

	//UFUNCTION(Server, Reliable)
	//void ServerTryMeleeAttack();

	//UFUNCTION(NetMulticast, Reliable)
	//void MulticastPlayMeleeAttackMontage(int32 MeleeAttackIndex);

	/*virtual void MeleeAttackCanCauseDamage(bool Value) override;*/

	/*virtual void MeleeAttackFinished() override;*/

	/*bool TrySetMeleeAttackState(EMeleeAttackState NewMeleeAttackState);*/

	
	FTransform GetMagicLaunchTransform();

	/*************************************************************************/
	/* Melee Weapon Block*/
	/*************************************************************************/

	UFUNCTION()
	void MeleeStartBlock();

	UFUNCTION()
	void MeleeStopBlock();

	//UFUNCTION(Server, Reliable)
	//void ServerTrySetWeaponBlocking(bool IsBlocking);

	UFUNCTION(BlueprintPure)
	bool IsBlocking();

	/*************************************************************************/
	/* Weapon Magic*/
	/*************************************************************************/

	/** [Server + Client] Player input to start using weapon magic */
	UFUNCTION()
	void StartUseWeaponMagic();

	///**
	// * [Server] Start player using weapon magic on server, will start animations on server used for MagicUseState progression
	// */
	//UFUNCTION(Server, Reliable)
	//void ServerStartUseWeaponMagic();

	///**
	// * [Server + Client] Play animations to start using weapon magic, animations used for MagicUseState progression will not be played
	// * as they are all played from ServerStartWeaponMagicProjectile
	// */
	//UFUNCTION(NetMulticast, Reliable)
	//void MulticastPlayStarUsetWeaponMagic();

	/** [Server + Client] Player input to finish using weapon magic  */
	UFUNCTION()
	void FinishUseWeaponMagic();

	///**
	// * [Server] Finish player using weapon magic on server, will start animation on server used to for MagicUseState progression
	// */
	//UFUNCTION(Server, Reliable)
	//void ServerFinishUseWeaponMagic();

	///**
	// * [Server + Client] Play animations to finish using weapon magic, animations used for MagicUseState progression will not be played
	// * as they are all played from ServerFinishUseWeaponMagic
	// */
	//UFUNCTION(NetMulticast, Reliable)
	//void MulticastPlayFinishUseWeaponMagic();

	///**
	// * [Server] Release the current weapon magic. Calls current melee weapon to react to magic charge released
	// */
	//UFUNCTION()
	//void ReleaseWeaponMagic();

	/*************************************************************************/
	/* ISMeleeWeaponWielder overloads*/
	/*************************************************************************/

	//UFUNCTION(BlueprintCallable)
	//virtual void ProjectileMagicAttack() override;

	//UFUNCTION(NetMulticast, Reliable)
	//void MulticastPlayProjectileMagicAttackMontage(int32 AttackIndex);

	//UFUNCTION()
	//virtual void SpawnMagicAttackProjectile() override;




	/*************************************************************************/
	/* ISMeleeWeaponWielder overloads*/
	/*************************************************************************/
	
	///** [Server] Called when the current equipped weapon changes its magic charge state */
	//virtual void WeaponMagicChargeChange(bool Value) override;

	//UFUNCTION()
	//bool TrySetMagicUseState(EMagicUseState NewMagicUseState);

	//UFUNCTION(BlueprintCallable)
	//FORCEINLINE EMagicUseState GetMagicUseState() const { return MagicUseState; }


	/*************************************************************************/
	/* Accessors */
	/*************************************************************************/

	/*UFUNCTION()
	FTransform GetFPPCameraTransform() const;*/


	virtual TWeakObjectPtr<USceneComponent> GetProjectileHomingTarget() override;

	/** [Server + Client] Play anim montage for local mesh */
	virtual float PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None) override;

	float PlayFirstPersonMontageOnServer(class UAnimMontage* AnimMontage);

	UFUNCTION(BlueprintCallable)
	bool IsMeleeWeaponMagicChargeReady();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UAnimInstance* GetLocalAnimInstance();

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

	///*************************************************************************/
	///* On Rep notifys*/
	///*************************************************************************/
	//UFUNCTION()
	//void OnRep_CurrentWeaponIsMagicCharged();

	/*************************************************************************/
	/* Helper Functions */
	/*************************************************************************/


	/**
	 * [Server] Helper function to check if current weapon in magic charged. Melee weapon is not replicated, will only be
	 * valid on server.
	 */
	bool IsCurrentMeleeWeaponMagicCharged() const;

	bool IsAIControlled() const;

	/**
	 * [Server + Client] Play matching animation on autonomous and simulated proxies. Do not use to play animations with
	 * notifies for used for game play progression
	 */
	void PlayMontagePairTPPandFPP(UAnimMontage* FPPMontage, UAnimMontage* TPPMontage) const;


	void SetProjectileHomingTargetLocation(bool IsCrouching);



};
