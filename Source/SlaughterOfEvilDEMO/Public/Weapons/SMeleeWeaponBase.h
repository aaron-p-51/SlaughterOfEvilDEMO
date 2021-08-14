// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SMeleeWeaponBase.generated.h"

//class UCapsuleComponent;
//class USphereComponent;
//class USMagicChargeComponent;
//class ASMagicProjectileBase;
//class UAnimMontage;
//class UParticleSystem;

////UENUM(BlueprintType)
////enum class EMeleeWeaponUseState : uint8
////{
////	EMWUS_Idle			UMETA(DisplayName="Idle"),
////	EMWUS_Attacking		UMETA(DisplayName="Attacking"),
////	EMWUS_Blocking		UMETA(DisplayName="Blocking")
////};
////
////
////UENUM(BlueprintType)
////enum class EMeleeWeaponChargeState : uint8
////{
////	EMWCS_Idle			UMETA(DisplayName="Idle"),
////	EMWCS_Charging		UMETA(DisplayName = "Charging"),
////	EMWCS_Charged		UMETA(DisplayName = "Charged"),
////	EMWCS_Finish		UMETA(DisplayName = "Finish")
////};
////
////
////USTRUCT()
////struct FMeleeWeaponAnims
////{
////	GENERATED_BODY()
////
////	/** Animation for owning pawn played for first person and server */
////	UPROPERTY(EditDefaultsOnly, Category = "Animation")
////	UAnimMontage* FirstPerson;
////
////	/** Animation for owning pawn played for third person */
////	UPROPERTY(EditDefaultsOnly, Category = "Animation")
////	UAnimMontage* ThirdPerson;
////	
////};
////
/////**
//// * Charge attacks are attacks where attack is started and held until released
//// * at a later point in time. There will be a looping animation needed
//// */
////USTRUCT(BlueprintType)
////struct FMeleeWeaponChargeAnims
////{
////	GENERATED_BODY()
////
////
////	UPROPERTY(EditDefaultsOnly, Category = "Animation")
////	FMeleeWeaponAnims Start;
////
////	UPROPERTY(EditDefaultsOnly, Category = "Animation")
////	FMeleeWeaponAnims Finish;
////
////	///** Animation for first person to start charge attack */
////	//UPROPERTY(EditDefaultsOnly, Category = "Animation")
////	//UAnimMontage* FirstPersonStart;
////
////	///** Animation for third person to start charge attack */
////	//UPROPERTY(EditDefaultsOnly, Category = "Animation")
////	//UAnimMontage* ThirdPersonStart;
////
////	///** Animation for first person to finish charge attack */
////	//UPROPERTY(EditDefaultsOnly, Category = "Animation")
////	//UAnimMontage* FirstPersonFinish;
////
////	///** Animation for third person to finish charge attack */
////	//UPROPERTY(EditDefaultsOnly, Category = "Animation")
////	//UAnimMontage* ThirdPersonFinish;
////};
////
/////**
//// * Melee Weapon data:
//// *		A FPP and TPP weapon will exist for each player. Depending of the players perspective only the proper weapon will be shown
//// *		All game play essential collisions will only be calculate from the FPPMeleeWeapon on the server.
//// */
////USTRUCT(BlueprintType)
////struct FMeleeWeaponData
////{
////	GENERATED_BODY()
////
////	UPROPERTY(EditDefaultsOnly, Category = "Damage")
////	float ApplyDamageForwardOffset = 100.f;
////
////	UPROPERTY(EditDefaultsOnly, Category = "Damage")
////	float ApplyDamageRadius = 50.f;
////
////	UPROPERTY(EditDefaultsOnly, Category = "Damage")
////	bool bDrawDebugDamageSphere;
////
////	UPROPERTY(EditDefaultsOnly, Category = "Damage")
////	float BaseDamage;
////
////	UPROPERTY(EditDefaultsOnly, Category = "Damage")
////	TSubclassOf<UDamageType> DamageTypeClass;
////
////	UPROPERTY(EditDefaultsOnly, Category = "Animation | Attacks")
////	TArray<FMeleeWeaponAnims> Attacks;
////
////	UPROPERTY(EditDefaultsOnly, Category = "Animation | ChargeAttacks")
////	TArray<FMeleeWeaponChargeAnims> ChargeAttacks;
////
////	UPROPERTY(EditDefaultsOnly, Category = "Animation | MagicChargeAttacks")
////	FMeleeWeaponChargeAnims MagicChargeAttack;
////
////	UPROPERTY(EditDefaultsOnly, Category = "Blocking")
////	FMeleeWeaponAnims BlockImpactMontage;
////
////	UPROPERTY(EditDefaultsOnly, Category = "Blocking")
////	FMeleeWeaponAnims StartBlockAnims;
////
////	UPROPERTY(EditDefaultsOnly, Category = "Blocking")
////	FMeleeWeaponAnims StopBlockAnims;
////
////	UPROPERTY(EditDefaultsOnly)
////	bool bCanMagicBlock;
////};
////
////USTRUCT(BlueprintType)
////struct FCosmeticCollisionPoints
////{
////	GENERATED_BODY()
////
////	UPROPERTY()
////	FVector PreviousLocation;
////
////	UPROPERTY()
////	FVector CurrentLocation;
////};


UCLASS(Abstract, Blueprintable)
class SLAUGHTEROFEVILDEMO_API ASMeleeWeaponBase : public AActor
{
	GENERATED_BODY()
	
/**
 * Members
 */
protected:

	///*************************************************************************/
	///* Weapon Data */
	///*************************************************************************/
	//
	//UPROPERTY(EditDefaultsOnly)
	//FMeleeWeaponData MeleeWeaponData;

	///** Current owner of this weapon */
	//UPROPERTY(Transient, ReplicatedUsing = OnRep_MyPawn)
	//class ASCharacterBase* MyPawn;

	//UPROPERTY(ReplicatedUsing=OnRep_MeleeAttackCount)
	//uint32 MeleeAttackCount = 0;

	//UPROPERTY(EditDefaultsOnly)
	//UParticleSystem* CosmeticCollisionBlood;

	//UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	//EMeleeWeaponUseState UseState;

	//UPROPERTY(ReplicatedUsing=OnRep_ChargeState, VisibleAnywhere, BlueprintReadOnly)
	//EMeleeWeaponChargeState ChargeState;

	///*************************************************************************/
	///* Components*/
	///*************************************************************************/

	///** RootComp so MeshComp can be offset */
	//UPROPERTY(VisibleDefaultsOnly)
	//USceneComponent* RootComp;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	//UStaticMeshComponent* MeshComp;

	///** Collision Comp  */
	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	//UCapsuleComponent* CosmeticCollisionComp;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	//USMagicChargeComponent* MagicChargeComp;

	//UPROPERTY(EditDefaultsOnly)
	//USphereComponent* MagicChargeCollisionComp;

	//UPROPERTY(EditDefaultsOnly)
	//USceneComponent* CollisionCompTest;



	///*************************************************************************/
	///* VFX */
	///*************************************************************************/

	//uint8 bCosmeticTraceEnable : 1;

	////FHitResult HitResult;

	///** Actors to ignore when doing weapon trace when MeleeWeaponState is EMWS_Attacking  */
	//UPROPERTY()
	//TArray<AActor*> CosmeticTraceIgnoreActors;

	///** Position of CollisionComp last frame */
	//FVector PreviousCosmeticTraceCenter;
	//
	//float TraceRadius;






	///*************************************************************************/
	///* Damage Calculations */
	///*************************************************************************/



	////// Cached Trace arguments for capsual trace
	////
	////float TraceHalfHeight;



	/////** Object types to detect when doing weapon trace when MeleeWeaponState is EMWS_Attacking */
	////UPROPERTY()
	////TArray<TEnumAsByte<EObjectTypeQuery>> DamageCollisionObjectTypes;


	//

	//
	//



	///*************************************************************************/
	///* Magic */
	///*************************************************************************/

	//UPROPERTY(EditDefaultsOnly)
	//TSubclassOf<ASMagicProjectileBase> DefaultMagicProjectile;

	//UPROPERTY(ReplicatedUsing=OnRep_IsBlocking)
	//uint8 bIsBlocking : 1;

	//UPROPERTY(ReplicatedUsing=OnRep_IsMagicCharged)
	//uint8 bIsMagicCharged : 1;


 /**
  * Methods
  */

public:	
	
	///** Sets default values for this actor's properties */
	//ASMeleeWeaponBase(); 

	//// Called every frame
	//virtual void Tick(float DeltaTime) override;

	///** [Server + Client] Set the owning pawn for this weapon */
	//void SetOwningPawn(ASCharacterBase* NewOwner);


	///*************************************************************************/
	///* Magic  */
	///*************************************************************************/

	///**
	// * [Server + Client] Apply effects to indicate magic charge state, effects should part of
	// * child class
	// */
	//UFUNCTION()
	//virtual void ApplyMagicChargeEffects() PURE_VIRTUAL(ASMeleeWeaponBase::ApplyMagicChargeEffects, );

	///**
	// * [Server + Client] Remove effects to indicate magic charge state, effects should part of
	// * child class
	// */
	//UFUNCTION()
	//virtual void RemoveMagicChargeEffects() PURE_VIRTUAL(ASMeleeWeaponBase::RemoveMagicChargeEffects, );

	///**
	// * [Server] Release magic charge from weapon
	// */
	//UFUNCTION()
	//virtual void ReleaseMagicCharge(FTransform ReleaseTransform) PURE_VIRTUAL(ASMeleeWeaponBase::ReleaseMagicCharge, );

	//UFUNCTION()
	//virtual void OnMagicChargeReady();

	//UFUNCTION()
	//virtual void OnMagicChargeRelease();


	///*************************************************************************/
	///* Accessors */
	///*************************************************************************/

	///**
	// * [Server + Client] Get current MeleeWeaponState
	// */
	//UFUNCTION()
	//virtual EMeleeWeaponUseState GetMeleeWeaponUseState() const;

	///**
	// * [Server] Get if this weapon is magic charged. Melee Weapon is not replicated only valid on server
	// */
	//UFUNCTION()
	//bool GetIsWeaponMagicCharged();

	//UFUNCTION()
	//bool GetIsWeaponMagicChargeReady();


	///*************************************************************************/
	///* Gameplay */
	///*************************************************************************/
	//
	///**
	// * [Server + Client] Set Weapon can cause damage, will change MeleeWeaponState to EMWS_Attacking
	// * if CanDamage is set to false will set MeleeWeaponState to EMWS_Idle.
	// * 
	// * @param CanDamage		Should Melee Weapon apply damage upon CollisionComp trace hit detected
	// * 
	// * return success if MeleeWeapon changed to EMWS_Idle when CanDamage is false, or EMWS_Attacking when CanDamage is true 
	// */
	///*virtual bool SetCanCauseDamage(bool CanDamage);*/

	///**
	//* [Server + Client] Set Weapon can cause damage, will change MeleeWeaponState to EMWS_Blocking
	//* if Blocking is set to false will set MeleeWeaponState to EMWS_Idle.
	//*
	//* @param Blocking		Should Melee Weapon enable CollisionComp to query to detect collisions
	//*
	//* return success if MeleeWeapon changed to EMWS_Idle when CanDamage is Blocking, or EMWS_Blocking when Blocking is true
	//*/
	///*virtual bool SetIsBlocking(bool Blocking);*/

	/////**
	//// * [Server + Client] Set Weapon Visibility, bWeaponVisibility
	//// */
	////FORCEINLINE void SetWeaponVisibility(bool Value) { bWeaponVisibility = Value; }

	//UFUNCTION()
	//void OnEquip();

	//UFUNCTION()
	//void OnEnterInventory(ASCharacterBase* NewOwner);

	//UFUNCTION()
	//void MeleeAttack();

	//UFUNCTION(Server, Reliable)
	//void ServerMeleeAttack();

	//UFUNCTION()
	//void TriggerOwnerPlayAttackAnimation(const FMeleeWeaponAnims& Animation);

	//UFUNCTION()
	//void OnAttackFinish();

	//bool WeaponIsIdle();

	//void TryApplyDamage();

	//void EnableCosmeticWeaponCollisions(bool Enable);

	///*UFUNCTION()
	//void OnCosmeticCollisionCompBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);*/

	//void StartBlock();

	//UFUNCTION(Server, Reliable)
	//void ServerStartBlock();


	//void StopBlock();

	//UFUNCTION(Server, Reliable)
	//void ServerStopBlock();


	//FORCEINLINE bool GetIsBlocking() const { return bIsBlocking; }



	//void StartUseMagicCharge();

	//UFUNCTION(Server, Reliable)
	//void ServerStartUseMagicCharge();

	//void FinishUseMagicCharge();

	//UFUNCTION(Server, Reliable)
	//void ServerFinishMagicCharge();





protected:

	///** Called when the game starts or when spawned */
	//virtual void BeginPlay() override;

	//UFUNCTION()
	//void OnRep_MyPawn();



	//UFUNCTION()
	//void OnLeaveInventory();



	//UFUNCTION()
	//void AttachMeshToPawn();

	//UFUNCTION()
	//void OnRep_MeleeAttackCount();

	//UFUNCTION()
	//void OnRep_ChargeState();

	//UFUNCTION()
	//void IncrementMeleeAttackCount();

	//UFUNCTION()
	//float PlayAnimMontageOnServer(UAnimMontage* Animation);

	//UFUNCTION()
	//void DisplayCosmeticVFX();

	//UFUNCTION()
	//void OnRep_IsBlocking();
	//
	//UFUNCTION()
	//void OnMagicChargeStateChange(bool IsMagicCharged);

	//UFUNCTION()
	//void EnableMagicChargeCollisions(bool Enable);

	//UFUNCTION()
	//void OnRep_IsMagicCharged();

private:

	///*************************************************************************/
	///* Compute and apply damage */
	///*************************************************************************/

	///**
	// * [Server] Check for a hit detected when weapon is attacking
	// * return was a hit detected
	// */
	//virtual bool CosmeticCollisionTrace(FHitResult& HitResult);

	///**
	// * [Server] Cache data need to perform trace in CheckForAttackTraceCollision
	// */
	//void CacheDamageTraceArguments();

	///*************************************************************************/
	///* Magic */
	///*************************************************************************/
	//
	/////**
	//// * [Server] Bound to MagicChargeComp delegate. Is called with magic charge state changes in MagicChargeComp
	//// */
	////UFUNCTION()
	////void OnMagicChargeChange(bool Charged);

	///**
	//* [Server + Client] Try and Set Melee Weapon State.
	//* @param NewMeleeWeaponState		Melee Weapon state to change to
	//*
	//* return success of setting NewMeleeWeaponState
	//*/
	//UFUNCTION()
	//virtual bool TrySetUseState(EMeleeWeaponUseState NewMeleeWeaponUseState);

	//UFUNCTION()
	//virtual bool TrySetChargeState(EMeleeWeaponChargeState NewMeleeWeaponChargeState);


	//void EnableCollisionsCosmeticCollisionComp(bool Enable);

};
