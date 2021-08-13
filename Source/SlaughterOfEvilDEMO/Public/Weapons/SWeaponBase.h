// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeaponBase.generated.h"


class UAnimMontage;
class ASCharacterBase;
class UStaticMeshComponent;
class USMagicChargeComponent;

USTRUCT()
struct FUseWeaponAnims
{
	GENERATED_BODY()

	/** Animation for owning pawn played for first person and server */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* FirstPerson;

	/** Animation for owning pawn played for third person */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* ThirdPerson;

};


UCLASS(Abstract, Blueprintable)
class SLAUGHTEROFEVILDEMO_API ASWeaponBase : public AActor
{
	GENERATED_BODY()
	

/**
 * Members
 */

protected:



	/** RootComp so MeshComp can be offset allows for same mesh socket for different weapons */
	UPROPERTY(VisibleDefaultsOnly)
	USceneComponent* RootComp;

	/** Mesh Component for weapon */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UStaticMeshComponent* MeshComp;

	/** Magic Charge component if this is not a magic charge weapon set bCanEverMagicCharge to false in derived BP class */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USMagicChargeComponent* MagicChargeComp;


	/** Current owner of this weapon */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_MyPawn)
	ASCharacterBase* MyPawn;

	UPROPERTY(ReplicatedUsing = OnRep_IsMagicCharged)
	uint8 bIsMagicCharged : 1;

public:	

	// Sets default values for this actor's properties
	ASWeaponBase();

	/** [Server + Client] Set the owning pawn for this weapon */
	void SetOwningPawn(ASCharacterBase* NewOwner);


	UFUNCTION()
	void OnEquip();


	UFUNCTION()
	void OnEnterInventory(ASCharacterBase* NewOwner);


	UFUNCTION()
	virtual void Use() PURE_VIRTUAL(ASWeaponBase::Use, );

	UFUNCTION()
	virtual void OnMagicChargeRelease() PURE_VIRTUAL(ASWeaponBase::OnMagicChargeRelease, )

	UFUNCTION()
	virtual void StartUseMagicCharge() PURE_VIRTUAL(ASWeaponBase::StartUseMagicCharge, );

	UFUNCTION()
	virtual void FinishUseMagicCharge() PURE_VIRTUAL(ASWeaponBase::FinishUseMagicCharge, );


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnRep_MyPawn();

	UFUNCTION()
	virtual void OnRep_IsMagicCharged();

	/**
	* [Server + Client] Apply effects to indicate magic charge state, effects should part of
	* child class
	*/
	UFUNCTION()
	virtual void ApplyMagicChargeEffects() PURE_VIRTUAL(ASWeaponBase::ApplyMagicChargeEffects, );

	/**
	 * [Server + Client] Remove effects to indicate magic charge state, effects should part of
	 * child class
	 */
	UFUNCTION()
	virtual void RemoveMagicChargeEffects() PURE_VIRTUAL(ASWeaponBase::RemoveMagicChargeEffects, );





	UFUNCTION()
	virtual void OnLeaveInventory();


	/**
	 * [Server + Client] Attach MeshComp to MyPawns weapon socket. At this point in development
	 * there is no need for third and first person player meshes to have different weapons. 
	 */
	UFUNCTION()
	virtual void AttachMeshToPawn();

	UFUNCTION()
	virtual void OnMagicChargeStateChange(bool IsMagicCharged) PURE_VIRTUAL(ASWeaponBase::OnMagicChargeStateChange, );

	UFUNCTION()
	virtual void TriggerOwnerPlayAttackAnimation(const FUseWeaponAnims& Animation);

	UFUNCTION()
	virtual float PlayAnimMontageOnServer(UAnimMontage* Animation);

	bool GetIsWeaponMagicCharged();


};
