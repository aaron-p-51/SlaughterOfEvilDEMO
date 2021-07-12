// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SMeleeWeaponBase.generated.h"

class UCapsuleComponent;
class USphereComponent;

UENUM(BlueprintType)
enum class EMeleeWeaponState : uint8
{
	EMWS_Idle			UMETA(DisplayName="Idle"),
	EMWS_Attacking		UMETA(DisplayName="Attacking"),
	EMWS_Blocking		UMETA(DisplayName="Blocking")
};

UENUM(BlueprintType)
enum class EMeleeWeaponPerspective : uint8
{
	EMWP_FirstPerson	UMETA(DisplayName="FirstPerson"),
	EMWP_ThirdPerson	UMETA(DisplayName="ThirdPerson")
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UCapsuleComponent* CollisionComp;

	/*************************************************************************/
	/* State Variables */
	/*************************************************************************/

	//UPROPERTY(ReplicatedUsing=OnRep_SetMagicCharge, VisibleDefaultsOnly, BlueprintReadOnly)
	//uint32 bIsMagicCharged : 1;

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

	FVector PreviousCollisionCenter;

	// Cached Trace arguments
	float TraceRadius;
	float TraceHalfHeight;

	UPROPERTY()
	TArray<AActor*> TraceIgnoreActors;
	UPROPERTY()
	TArray<TEnumAsByte<EObjectTypeQuery>> CollisionObjectTypes;

	FHitResult HitResult;


	/*************************************************************************/
	/* Gameplay */
	/*************************************************************************/

	EMeleeWeaponPerspective MeleeWeaponPerspective;


 /**
  * Methods
  */

public:	
	
	/** Sets default values for this actor's properties */
	ASMeleeWeaponBase();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/*************************************************************************/
	/* Change Magic State */
	/*************************************************************************/

	//UFUNCTION()
	//virtual void ApplyMagicCharge() PURE_VIRTUAL(ASMeleeWeaponBase::ApplyMagicCharge, );

	//UFUNCTION()
	//virtual void RemoveMagicCharge() PURE_VIRTUAL(ASMeleeWeaponBase::RemoveMagicCharge, );

	//UFUNCTION()
	//virtual bool TrySetMagicCharge(bool Charged);

	UFUNCTION()
	virtual bool TrySetMeleeWeaponState(EMeleeWeaponState NewMeleeWeaponState);


	/*************************************************************************/
	/* Accessors */
	/*************************************************************************/

	//UFUNCTION()
	//virtual bool IsMagicCharged() const;

	UFUNCTION()
	virtual EMeleeWeaponState GetMeleeWeaponState() const;

	/*************************************************************************/
	/* Gameplay */
	/*************************************************************************/
	
	virtual void SetCanCauseDamage(bool CanDamage);

	virtual void SetIsBlocking(bool Blocking);

	void SetMeleeWeaponPerspective(EMeleeWeaponPerspective Perspective);


protected:

	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;

	//UFUNCTION()
	//virtual void OnRep_SetMagicCharge();

private:

	virtual bool CheckForCollision();

	void CacheDamageTraceArguments();



};
