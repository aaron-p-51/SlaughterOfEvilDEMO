// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SMeleeWeaponWielder.h"
#include "SCharacterBase.generated.h"



class UCameraComponent;
class ASMeleeWeaponBase;

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
	EMUS_NoCharge		UMETA(DisplayName = "NoCharge"),
	EMUS_Idle			UMETA(DisplayName = "Idle"),
	EMUS_Start			UMETA(DisplayName = "Start"),
	EMUS_Ready			UMETA(DisplayName = "Ready"),
	EMUS_Finish			UMETA(DisplayName = "Finish")
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
	UPROPERTY(EditDefaultsOnly)
	TMap<EMeleeAttackDirection, UAnimMontage*> FirstPeronAttackMontages;

	/**
	 * Attach montages for TPP. Montages are only for aesthetics on Simulated proxies.
	 */
	UPROPERTY(EditDefaultsOnly)
	TMap<EMeleeAttackDirection, UAnimMontage*> ThirdPeronAttackMontages;

	/** Impact montage when weapon is blocking, will only play on locally controlled character (FPP) */
	UPROPERTY(EditDefaultsOnly)
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
	/* Weapon Attack*/
	/*************************************************************************/
	UFUNCTION()
	void WeaponAttack();

	UFUNCTION(Server, Reliable)
	void ServerTryMeleeAttack();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayMeleeAttackMontage(EMeleeAttackDirection MeleeAttack);

	/*************************************************************************/
	/* Weapon Block*/
	/*************************************************************************/

	UFUNCTION()
	void WeaponBlockStart();

	UFUNCTION()
	void WeaponBlockStop();

	UFUNCTION(Server, Reliable)
	void ServerTrySetWeaponBlocking(bool IsBlocking);

	UFUNCTION(BlueprintPure)
	virtual bool IsBlocking() override;

	/*************************************************************************/
	/* Weapon Magic*/
	/*************************************************************************/

	UFUNCTION()
	void StartWeaponMagicProjectile();

	UFUNCTION(Server, Reliable)
	void ServerStartWeaponMagicProjectile();

	UFUNCTION()
	void FinishWeaponMagicProjectile();

	UFUNCTION(Server, Reliable)
	void ServerFinishWeaponMagicProjectile(float ProjectilePitch);



	/*************************************************************************/
	/* ISMeleeWeaponWielder overloads*/
	/*************************************************************************/
	
	/** Called when the current equipped weapon changes its magic charge state */
	virtual void WeaponMagicChargeChange(bool Value) override;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



private:	

	/*************************************************************************/
	/* Anim notify call backs*/
	/*************************************************************************/
	UFUNCTION()
	void OnMontageNotifyBeginTryApplyDamage(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	UFUNCTION()
	void OnMontageNotifyEndSetWeaponIdleState(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

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

	UFUNCTION()
	bool TrySetMagicUseState(EMagicUseState NewMagicUseState);

};
