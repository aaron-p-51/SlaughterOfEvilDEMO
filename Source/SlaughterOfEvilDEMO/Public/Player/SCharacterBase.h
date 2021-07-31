// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SMeleeWeaponWielder.h"
#include "SCharacterBase.generated.h"



class UCameraComponent;
class ASMeleeWeaponBase;


UENUM(BlueprintType)
enum class EMeleeAttackState : uint8
{
	EMAS_Idle			UMETA(DisplayName = "Idle"),
	EMAS_Attacking		UMETA(DisplayName = "Attacking"),
	EMAS_CauseDamage	UMETA(DisplayName = "CauseDamage")
};

/**
 * Melee Attach swing directions
 */
UENUM(BlueprintType)
enum class EMeleeAttackDirection : uint8
{
	EMAD_Left			UMETA(DisplayName = "Left"),
	EMAD_LeftDown		UMETA(DisplayName = "LeftDown"),
	EMAD_Right			UMETA(DisplayName = "Right"),
	EMAD_RightDown		UMETA(DisplayName = "RightDown")
};

UENUM(BlueprintType)
enum class EMagicUseState : uint8
{

	EMUS_NoCharge		UMETA(DisplayName = "NoCharge"),	// Weapon is not magic charged
	EMUS_Idle			UMETA(DisplayName = "Idle"),		// Weapon is magic charged has not starting using magic
	EMUS_Start			UMETA(DisplayName = "Start"),		// Weapon is magic charged, staring to use (charing)
	EMUS_Ready			UMETA(DisplayName = "Ready"),		// Weapon is magic charged, ready to use (charged)
	EMUS_Finish			UMETA(DisplayName = "Finish")		// Weapon is magic charged, signal release of magic
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

	/** Parent class of melee weapons (ASMeleeWeaponBase) */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ASMeleeWeaponBase> MeleeWeaponClass;

	/** Melee weapon for TPP, this will only be visible for simulated proxies */
	UPROPERTY(VisibleAnywhere)
	ASMeleeWeaponBase* TPPMeleeWeapon;

	/** Melee weapon for TPP, used for all game play essentials calculations and collisions */
	UPROPERTY(VisibleAnywhere);
	ASMeleeWeaponBase* FPPMeleeWeapon;

	/** Is this the starting weapon, if multiple are set the first weapon spawned from MeleeWeaponData will be set as starting Weapon */
	UPROPERTY(EditDefaultsOnly)
	bool bIsStartingWeapon;

	/** Socket on mesh where to attach MeleeWeapon and FPPMeleeWeapon, Socket needs to exist on both on Mesh and FirstPersonMesh */
	UPROPERTY(EditDefaultsOnly)
	FName MeleeWeaponSocketName = FName(TEXT("RightHandMeleeWeaponSocket"));

	/**
	 * Attach montages for FPP. Montages should include notify begin and end points to indicate when damage can be applied
	 * These montages will play on players character on server
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Attack | First Person")
	TMap<EMeleeAttackDirection, UAnimMontage*> FPPAttackMontages;

	/**
	 * Attach montages for TPP. Montages are only for aesthetics on Simulated proxies.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Attack | Third Person")
	TMap<EMeleeAttackDirection, UAnimMontage*> TPPAttackMontages;

	/** Animation montage for FPP when starting to use magic (charging to use) */
	UPROPERTY(EditDefaultsOnly, Category = "Attack | First Person")
	UAnimMontage* FPPMagicStartMontage;

	/** Animation montage for FPP to use magic, done after fully charged (FPPMagicStartMontage)  */
	UPROPERTY(EditDefaultsOnly, Category = "Attack | First Person")
	UAnimMontage* FPPMagicFinishMontage;

	/** Animation montage for TPP when starting to use magic (charging to use) */
	UPROPERTY(EditDefaultsOnly, Category = "Attack | Third Person")
	UAnimMontage* TPPMagicStartMontage;

	/** Animation montage for TPP when starting to use magic (charging to use) */
	UPROPERTY(EditDefaultsOnly, Category = "Attack | Third Person")
	UAnimMontage* TPPMagicFinishMontage;

	/** Impact montage when weapon is blocking, will only play on locally controlled character (FPP) */
	UPROPERTY(EditDefaultsOnly, Category = "Block | First Person")
	UAnimMontage* BlockImpactMontage;
};

UCLASS()
class SLAUGHTEROFEVILDEMO_API ASCharacterBase : public ACharacter, public ISMeleeWeaponWielder
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
	TArray<FMeleeWeaponData> MeleeWeaponData;

	/** Current weapon being used from MeleeWeaponData */
	uint32 CurrentWeaponIndex;

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

	UPROPERTY(Replicated)
	uint32 bIsBlocking : 1;

	UPROPERTY(ReplicatedUsing=OnRep_CurrentWeaponIsMagicCharged)
	uint32 bCurrentWeaponIsMagicCharged : 1;

	UPROPERTY(Replicated)
	EMagicUseState MagicUseState;

	EMeleeAttackState MeleeAttackState;


private:

	/** Last melee attack direction TODO: remove this and make next melee attack deterministic (faster control for clients) */
	EMeleeAttackDirection MeleeAttackDirection;

 /**
  * Methods
  */
public:
	// Sets default values for this character's properties
	ASCharacterBase();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

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
	virtual void MeleeAttack() override;

	UFUNCTION(Server, Reliable)
	void ServerTryMeleeAttack();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayMeleeAttackMontage(EMeleeAttackDirection MeleeAttack);

	bool TrySetMeleeAttackState(EMeleeAttackState NewMeleeAttackState);


	/*************************************************************************/
	/* Melee Weapon Block*/
	/*************************************************************************/

	UFUNCTION()
	virtual void MeleeBlockStart() override;

	UFUNCTION()
	virtual void MeleeBlockStop() override;

	UFUNCTION(Server, Reliable)
	void ServerTrySetWeaponBlocking(bool IsBlocking);

	UFUNCTION(BlueprintPure)
	virtual bool IsBlocking() override;

	/*************************************************************************/
	/* Weapon Magic*/
	/*************************************************************************/

	/** [Server + Client] Player input to start using weapon magic */
	UFUNCTION()
	void StartUseWeaponMagic();

	/**
	 * [Server] Start player using weapon magic on server, will start animations on server used for MagicUseState progression
	 */
	UFUNCTION(Server, Reliable)
	void ServerStartUseWeaponMagic();

	/**
	 * [Server + Client] Play animations to start using weapon magic, animations used for MagicUseState progression will not be played
	 * as they are all played from ServerStartWeaponMagicProjectile
	 */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayStarUsetWeaponMagic();

	/** [Server + Client] Player input to finish using weapon magic  */
	UFUNCTION()
	void FinishUseWeaponMagic();

	/**
	 * [Server] Finish player using weapon magic on server, will start animation on server used to for MagicUseState progression
	 */
	UFUNCTION(Server, Reliable)
	void ServerFinishUseWeaponMagic();

	/**
	 * [Server + Client] Play animations to finish using weapon magic, animations used for MagicUseState progression will not be played
	 * as they are all played from ServerFinishUseWeaponMagic
	 */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayFinishUseWeaponMagic();

	/**
	 * [Server] Release the current weapon magic. Calls current melee weapon to react to magic charge released
	 */
	UFUNCTION()
	void ReleaseWeaponMagic();


	/*************************************************************************/
	/* ISMeleeWeaponWielder overloads*/
	/*************************************************************************/
	
	/** [Server] Called when the current equipped weapon changes its magic charge state */
	virtual void WeaponMagicChargeChange(bool Value) override;

	UFUNCTION()
	bool TrySetMagicUseState(EMagicUseState NewMagicUseState);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE EMagicUseState GetMagicUseState() const { return MagicUseState; }


	/*************************************************************************/
	/* Accessors */
	/*************************************************************************/

	/*UFUNCTION()
	FTransform GetFPPCameraTransform() const;*/

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



private:	


	/*************************************************************************/
	/* On Rep notifys*/
	/*************************************************************************/
	UFUNCTION()
	void OnRep_CurrentWeaponIsMagicCharged();

	/*************************************************************************/
	/* Helper Functions */
	/*************************************************************************/

	EMeleeAttackDirection GetMeleeAttackDirection() const;

	/** Spawn starting weapons, all weapons in MeleeWeaponData will be spawned and attach the FPP and TPP meshes */
	UFUNCTION(BlueprintCallable)
	void SpawnStartingWeapons();

	/**
	 * [Server] Helper function to check if current weapon in magic charged. Melee weapon is not replicated, will only be
	 * valid on server.
	 */
	bool IsCurrentMeleeWeaponMagicCharged() const;

	/**
	 * [Server + Client] Play matching animation on autonomous and simulated proxies. Do not use to play animations with
	 * notifies for used for game play progression
	 */
	void PlayMontagePairTPPandFPP(UAnimMontage* FPPMontage, UAnimMontage* TPPMontage) const;



};
