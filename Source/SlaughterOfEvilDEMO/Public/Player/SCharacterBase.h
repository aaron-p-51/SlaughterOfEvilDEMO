// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacterBase.generated.h"


class UCameraComponent;
class ASMeleeWeaponBase;

UENUM(BlueprintType)
enum class EMeleeAttackDirection : uint8
{
	EMAD_Left			UMETA(DisplayName = "Left"),
	EMAD_LeftDown		UMETA(DisplayName = "LeftDown"),
	EMAD_Right			UMETA(DisplayName = "Right"),
	EMAD_RightDown		UMETA(DisplayName = "RightDown")
};

USTRUCT(BlueprintType)
struct FWeaponData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ASMeleeWeaponBase> MeleeWeaponClass;

	UPROPERTY(VisibleAnywhere)
	ASMeleeWeaponBase* MeleeWeapon;

	UPROPERTY(EditDefaultsOnly)
	bool bIsStartingWeapon;

	UPROPERTY(EditDefaultsOnly)
	FName MeleeWeaponSocketName = FName(TEXT("RightHandMeleeWeaponSocket"));

	UPROPERTY(EditDefaultsOnly)
	float WeaponUseVerticalOffset;

	UPROPERTY(EditDefaultsOnly)
	float WeaponUseForwardOffset;

};

UCLASS()
class SLAUGHTEROFEVILDEMO_API ASCharacterBase : public ACharacter
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
	
	/*************************************************************************/
	/* Weapons*/
	/*************************************************************************/

	UPROPERTY(EditDefaultsOnly, Category = "Configuration | Weapons")
	TArray<FWeaponData> WeaponData;

	UPROPERTY()
	int32 CurrentWeapon;


	/*************************************************************************/
	/* Animation */
	/*************************************************************************/

	UPROPERTY(VisibleDefaultsOnly, Category = "Configuration | Animation")
	UAnimInstance* FirstPersonAnimInstance;

	UPROPERTY(VisibleDefaultsOnly, Category = "Configuration | Animation")
	UAnimInstance* ThirdPersonAnimInstance;

	UPROPERTY(EditDefaultsOnly, Category = "Configuration | Animation")
	TMap<EMeleeAttackDirection, UAnimMontage*> FirstPersonLongswordMontages;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* MeleeAttackMontage;

private:

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

	UFUNCTION()
	void MoveForward(float Value);
	void MoveRight(float Value);

	UFUNCTION()
	void WeaponAction();

	UFUNCTION(Server, Reliable)
	void ServerTryMeleeAttack();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayMeleeAttackMontage(EMeleeAttackDirection MeleeAttack);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


private:	

	EMeleeAttackDirection GetMeleeAttackDirection() const;

	UFUNCTION()
	void OnMontageNotifyBeginTryApplyDamage(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	UFUNCTION()
	void OnMontageNotifyEndSetWeaponIdleState(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	/*************************************************************************/
	/* Helper Functions */
	/*************************************************************************/

	void SpawnStartingWeapons();

};
