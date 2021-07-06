// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SMeleeWeaponBase.generated.h"


UENUM(BlueprintType)
enum class EMeleeWeaponState : uint8
{
	EMWS_Idle			UMETA(DisplayName="Idle"),
	EMWS_Attacking		UMETA(DisplayName="Attacking"),
	EMWS_Blocking		UMETA(DisplayName="Blocking")
};

USTRUCT(BlueprintType)
struct FApplyDamageData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float BaseDamage;

	/** Damage will be applied to any actors that  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DamageSphereRadius;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageType> DamageTypeClass;

};

UCLASS(Abstract, Blueprintable)
class SLAUGHTEROFEVILDEMO_API ASMeleeWeaponBase : public AActor
{
	GENERATED_BODY()
	
/**
 * Members
 */

protected:

	/*************************************************************************/
	/* Components*/
	/*************************************************************************/

	UPROPERTY(VisibleDefaultsOnly)
	USceneComponent* RootComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UStaticMeshComponent* MeshComp;

	/*************************************************************************/
	/* State Variables */
	/*************************************************************************/

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	uint32 bIsMagicCharged : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EMeleeWeaponState MeleeWeaponState;

	/*************************************************************************/
	/* Damage */
	/*************************************************************************/
	
	FApplyDamageData ApplyDamageData;

	UPROPERTY(EditDefaultsOnly)
	bool bDrawDebugHitSphere;

	UPROPERTY(EditDefaultsOnly)
	TArray<TEnumAsByte<EObjectTypeQuery>> OverlappingObjectTypes;

	/*************************************************************************/
	/* Gameplay */
	/*************************************************************************/

	//UPROPERTY(VisibleAnywhere)
	//AActor* MyOwner;

 /**
  * Methods
  */

public:	
	
	/** Sets default values for this actor's properties */
	ASMeleeWeaponBase();


	/*************************************************************************/
	/* Change Magic State */
	/*************************************************************************/

	UFUNCTION()
	virtual void ApplyMagicCharge() PURE_VIRTUAL(ASMeleeWeaponBase::ApplyMagicCharge, );

	UFUNCTION()
	virtual void RemoveMagicCharge() PURE_VIRTUAL(ASMeleeWeaponBase::RemoveMagicCharge, );

	UFUNCTION()
	virtual bool SetMagicChargeState(bool Charged);

	UFUNCTION()
	virtual bool TrySetMeleeWeaponState(EMeleeWeaponState NewMeleeWeaponState);


	/*************************************************************************/
	/* Accessors */
	/*************************************************************************/

	UFUNCTION()
	virtual bool IsMagicCharged() const;

	UFUNCTION()
	virtual EMeleeWeaponState GetMeleeWeaponState() const;

	/*************************************************************************/
	/* Damage */
	/*************************************************************************/
	
	UFUNCTION()
	virtual bool TryApplyDamage(FVector& ApplyDamageCenter) const;

	void SetApplyDamageData(FApplyDamageData DamageData);

protected:

	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;


};
