//// Fill out your copyright notice in the Description page of Project Settings.
//
//
#include "Weapons/SMeleeWeaponBase.h"
//
//// Engine Includes
//#include "DrawDebugHelpers.h"
//#include "Engine/CollisionProfile.h"
//#include "Kismet/KismetSystemLibrary.h"
//#include "Kismet/GameplayStatics.h"
//#include "Components/CapsuleComponent.h"
//#include "Components/SphereComponent.h"
//#include "Math/UnrealMathUtility.h"
//#include "Net/UnrealNetwork.h"
//#include "Particles/ParticleSystem.h"
//
//
//// Game Includes
//#include "Player/SCharacterBase.h"
//#include "Components/SMagicChargeComponent.h"
//
//static const float ANIM_PLAY_FAIL = 0.f;
//
//// Sets default values
//ASMeleeWeaponBase::ASMeleeWeaponBase()
//{
//	PrimaryActorTick.bCanEverTick = true;
//
//	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
//	if (RootComp)
//	{
//		SetRootComponent(RootComp);
//	}
//
//	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
//	if (MeshComp && RootComp)
//	{
//		MeshComp->SetupAttachment(RootComp);
//	}
//
//	CosmeticCollisionComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CosmeticCollisionComp"));
//	if (CosmeticCollisionComp)
//	{
//		CosmeticCollisionComp->SetupAttachment(GetRootComponent());
//		CosmeticCollisionComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
//		CosmeticCollisionComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
//		CosmeticCollisionComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
//		CosmeticCollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
//	}
//
//	CollisionCompTest = CreateDefaultSubobject<USceneComponent>(TEXT("CollisionCompTest"));
//	if (CollisionCompTest)
//	{
//		CollisionCompTest->SetupAttachment(GetRootComponent());
//	}
//
//	MagicChargeCollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("MagicChargeCollisionComp"));
//	if (MagicChargeCollisionComp)
//	{
//		//MagicChargeCollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
//		MagicChargeCollisionComp->SetupAttachment(GetRootComponent());
//		MagicChargeCollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
//		MagicChargeCollisionComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
//		MagicChargeCollisionComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
//		MagicChargeCollisionComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
//	}
//
//	MagicChargeComp = CreateDefaultSubobject<USMagicChargeComponent>(TEXT("MagicChargeComp"));
//
//	/*bWeaponVisibility = true;*/
//
//	bReplicates = true;
//}
//
//
//void ASMeleeWeaponBase::BeginPlay()
//{
//	Super::BeginPlay();
//	
//	/*if (MeshComp)
//	{
//		MeshComp->SetVisibility(bWeaponVisibility);
//	}*/
//
//	if (GetLocalRole() == ENetRole::ROLE_Authority)
//	{
//		if (MagicChargeComp)
//		{
//			MagicChargeComp->OnMagicChargeChange.AddDynamic(this, &ASMeleeWeaponBase::OnMagicChargeStateChange);
//		}
//
//
//
//		CacheDamageTraceArguments();
//	}
//
//
//	
//}
//
//
//
//
//void ASMeleeWeaponBase::CacheDamageTraceArguments()
//{
//	if (CosmeticCollisionComp)
//	{
//		TraceRadius = CosmeticCollisionComp->GetScaledCapsuleRadius();
//		/*TraceHalfHeight = CosmeticCollisionComp->GetScaledCapsuleHalfHeight();*/
//		/*CosmeticCollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ASMeleeWeaponBase::OnCosmeticCollisionCompBeginOverlap);*/
//	}
//
//	if (MagicChargeComp)
//	{
//		MagicChargeComp->SetCanMagicCharge(MeleeWeaponData.bCanMagicBlock);
//		MagicChargeComp->OnMagicChargeChange.AddDynamic(this, &ASMeleeWeaponBase::OnMagicChargeStateChange);
//	}
//	
//	//DamageCollisionObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
//}
//
//
//void ASMeleeWeaponBase::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//	if (bCosmeticTraceEnable)
//	{
//		DisplayCosmeticVFX();
//	}
//}
//
//
//void ASMeleeWeaponBase::DisplayCosmeticVFX()
//{
//	FHitResult HitResult;
//	bool Hit = CosmeticCollisionTrace(HitResult);
//	if (Hit && CosmeticCollisionBlood)
//	{
//		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), CosmeticCollisionBlood, HitResult.ImpactPoint, (HitResult.ImpactNormal * -1.f).Rotation());
//		bCosmeticTraceEnable = false;
//	}
//}
//
//
//
//
//bool ASMeleeWeaponBase::CosmeticCollisionTrace(FHitResult& HitResult)
//{
//	bool bHit = false;
//	if (CollisionCompTest)
//	{
//		FVector CurrentCosmeticTraceCenter = CollisionCompTest->GetComponentLocation();
//
//		ETraceTypeQuery TraceTypeQuery = UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel2);
//		bHit = UKismetSystemLibrary::SphereTraceSingle(
//			this,
//			PreviousCosmeticTraceCenter,
//			CurrentCosmeticTraceCenter,
//			TraceRadius,
//			TraceTypeQuery,
//			true,
//			CosmeticTraceIgnoreActors,
//			EDrawDebugTrace::ForOneFrame,
//			HitResult,
//			true
//		);
//		PreviousCosmeticTraceCenter = CurrentCosmeticTraceCenter;
//	}
//
//	return bHit;
//}
//
//
////void ASMeleeWeaponBase::OnMagicChargeChange(bool Value)
////{
////	auto MyOwner = GetOwner();
////	if (MyOwner)
////	{
////		auto MeleeWeaponWielder = Cast<ISMeleeWeaponWielder>(MyOwner);
////		if (MeleeWeaponWielder)
////		{
////			MeleeWeaponWielder->WeaponMagicChargeChange(Value);
////		}
////	}
////}
//
//
//bool ASMeleeWeaponBase::TrySetUseState(EMeleeWeaponUseState NewMeleeWeaponUseState)
//{
//	//
//	// Apply and state change conditions here
//	//
//	bool ValidStateChange = false;
//	if (GetLocalRole() == ENetRole::ROLE_Authority)
//	{
//		if (UseState != EMeleeWeaponUseState::EMWUS_Attacking && NewMeleeWeaponUseState == EMeleeWeaponUseState::EMWUS_Attacking)
//		{
//			ValidStateChange = true;
//		}
//		else if (UseState == EMeleeWeaponUseState::EMWUS_Idle && NewMeleeWeaponUseState == EMeleeWeaponUseState::EMWUS_Blocking)
//		{
//			ValidStateChange = true;
//		}
//		else if (NewMeleeWeaponUseState == EMeleeWeaponUseState::EMWUS_Idle)
//		{
//			ValidStateChange = true;
//		}
//
//		if (ValidStateChange)
//		{
//			UseState = NewMeleeWeaponUseState;
//		}
//	}
//	return ValidStateChange;
//}
//
//
//
//
//EMeleeWeaponUseState ASMeleeWeaponBase::GetMeleeWeaponUseState() const
//{
//	return UseState;
//}
//
//
//bool ASMeleeWeaponBase::GetIsWeaponMagicCharged()
//{
//	return (MagicChargeComp) ? MagicChargeComp->IsMagicCharged() : false;
//}
//
//
//
//
//bool ASMeleeWeaponBase::GetIsWeaponMagicChargeReady()
//{
//	return (ChargeState == EMeleeWeaponChargeState::EMWCS_Charged);
//}
//
//#pragma region Melee Attack
//
//void ASMeleeWeaponBase::MeleeAttack()
//{
//	if (WeaponIsIdle())
//	{
//		if (GetLocalRole() < ENetRole::ROLE_Authority)
//		{
//			ServerMeleeAttack();
//		}
//
//		IncrementMeleeAttackCount();
//		TriggerOwnerPlayAttackAnimation(MeleeWeaponData.Attacks[MeleeAttackCount]);
//
//		if (GetLocalRole() == ENetRole::ROLE_Authority)
//		{
//			if (PlayAnimMontageOnServer(MeleeWeaponData.Attacks[MeleeAttackCount].FirstPerson) > ANIM_PLAY_FAIL)
//			{
//				TrySetUseState(EMeleeWeaponUseState::EMWUS_Attacking);
//			}
//		}
//	}
//}
//
//
//void ASMeleeWeaponBase::ServerMeleeAttack_Implementation()
//{
//	MeleeAttack();
//}
//
//
//void ASMeleeWeaponBase::TriggerOwnerPlayAttackAnimation(const FMeleeWeaponAnims& Animation)
//{
//	if (MyPawn)
//	{
//		auto PerspectiveAnimation = MyPawn->IsFirstPerson() ? Animation.FirstPerson : Animation.ThirdPerson;
//		if (PerspectiveAnimation)
//		{
//			MyPawn->PlayAnimMontage(PerspectiveAnimation);
//		}
//	}
//}
//
//
//void ASMeleeWeaponBase::OnRep_MeleeAttackCount()
//{
//	if (MyPawn && MyPawn->IsLocallyControlled() == false)
//	{
//		TriggerOwnerPlayAttackAnimation(MeleeWeaponData.Attacks[MeleeAttackCount]);
//	}
//}
//
//
//void ASMeleeWeaponBase::OnRep_ChargeState()
//{
//	if (MyPawn && MyPawn->IsLocallyControlled() == false)
//	{
//		if (ChargeState == EMeleeWeaponChargeState::EMWCS_Charging)
//		{
//			TriggerOwnerPlayAttackAnimation(MeleeWeaponData.MagicChargeAttack.Start);
//		}
//		else if (ChargeState == EMeleeWeaponChargeState::EMWCS_Finish)
//		{
//			TriggerOwnerPlayAttackAnimation(MeleeWeaponData.MagicChargeAttack.Finish);
//		}
//	}
//}
//
//void ASMeleeWeaponBase::IncrementMeleeAttackCount()
//{
//	MeleeAttackCount = ++MeleeAttackCount % MeleeWeaponData.Attacks.Num();
//}
//
//
//float ASMeleeWeaponBase::PlayAnimMontageOnServer(UAnimMontage* Animation)
//{
//	if (MyPawn && Animation)
//	{
//		return MyPawn->PlayFirstPersonMontageOnServer(Animation);
//	}
//	return ANIM_PLAY_FAIL;
//}
//
//
//
//void ASMeleeWeaponBase::OnAttackFinish()
//{
//	TrySetUseState(EMeleeWeaponUseState::EMWUS_Idle);
//}
//
//
//bool ASMeleeWeaponBase::WeaponIsIdle()
//{
//	return UseState == EMeleeWeaponUseState::EMWUS_Idle;
//}
//
//
//void ASMeleeWeaponBase::TryApplyDamage()
//{
//	FVector ApplyDamageCenter = MyPawn->GetActorLocation() + MyPawn->GetActorForwardVector() * MeleeWeaponData.ApplyDamageForwardOffset;
//	TArray<AActor*> IgnoreActors;
//	IgnoreActors.Add(this);
//	IgnoreActors.Add(MyPawn);
//	TArray<AActor*> OverlapActors;
//	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
//	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
//	
//	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), ApplyDamageCenter, MeleeWeaponData.ApplyDamageRadius, ObjectTypes, nullptr, 
//		IgnoreActors, OverlapActors);
//
//	if (MeleeWeaponData.bDrawDebugDamageSphere)
//	{
//		DrawDebugSphere(GetWorld(), ApplyDamageCenter, MeleeWeaponData.ApplyDamageRadius, 15, FColor::Red, false, 3.f, 0, 3.f);
//	}
//
//	for (auto& Actor : OverlapActors)
//	{
//		// Apply Damage
//	}
//}
//
//
//void ASMeleeWeaponBase::EnableCosmeticWeaponCollisions(bool Enable)
//{
//	if (Enable && UseState == EMeleeWeaponUseState::EMWUS_Attacking)
//	{
//		EnableCollisionsCosmeticCollisionComp(true);
//		bCosmeticTraceEnable = true;
//		PreviousCosmeticTraceCenter = CollisionCompTest->GetComponentLocation();
//		CosmeticTraceIgnoreActors.AddUnique(this);
//		CosmeticTraceIgnoreActors.AddUnique(MyPawn);
//	}
//	else
//	{
//		EnableCollisionsCosmeticCollisionComp(false);
//		bCosmeticTraceEnable = false;
//	}
//}
//
//
//
//
//bool ASMeleeWeaponBase::TrySetChargeState(EMeleeWeaponChargeState NewMeleeWeaponChargeState)
//{
//	bool ValidStateChange = false;
//	if (GetLocalRole() == ENetRole::ROLE_Authority)
//	{
//		if ((bIsMagicCharged && ChargeState == EMeleeWeaponChargeState::EMWCS_Idle		&& NewMeleeWeaponChargeState == EMeleeWeaponChargeState::EMWCS_Charging) ||
//			(bIsMagicCharged && ChargeState == EMeleeWeaponChargeState::EMWCS_Charging	&& NewMeleeWeaponChargeState == EMeleeWeaponChargeState::EMWCS_Charged)	||
//			(ChargeState == EMeleeWeaponChargeState::EMWCS_Charged	&& NewMeleeWeaponChargeState == EMeleeWeaponChargeState::EMWCS_Finish) ||
//			NewMeleeWeaponChargeState == EMeleeWeaponChargeState::EMWCS_Idle)
//		{
//			ValidStateChange = true;
//		}
//
//		/*if (ChargeState == EMeleeWeaponChargeState::EMWCS_Idle && NewMeleeWeaponChargeState == EMeleeWeaponChargeState::EMWCS_Charging)
//		{
//			ValidStateChange = true;
//		}
//		else if (ChargeState == EMeleeWeaponChargeState::EMWCS_Charging && NewMeleeWeaponChargeState == EMeleeWeaponChargeState::EMWCS_Charged)
//		{
//			ValidStateChange = true;
//		}
//		else if (ChargeState == EMeleeWeaponChargeState::EMWCS_Charged && NewMeleeWeaponChargeState == EMeleeWeaponChargeState::EMWCS_Finish)
//		{
//			ValidStateChange = true;
//		}
//		else if (NewMeleeWeaponChargeState == EMeleeWeaponChargeState::EMWCS_Idle)
//		{
//			ValidStateChange = true;
//		}*/
//
//		if (ValidStateChange)
//		{
//			ChargeState = NewMeleeWeaponChargeState;
//		}
//	}
//
//	return ValidStateChange;
//}
//
//void ASMeleeWeaponBase::EnableCollisionsCosmeticCollisionComp(bool Enable)
//{
//	if (!CosmeticCollisionComp) return;
//
//	if (Enable)
//	{
//		CosmeticCollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
//	}
//	else
//	{
//		CosmeticCollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
//	}
//}
//
//#pragma endregion
//
//
//#pragma region Blocking
//
//
//void ASMeleeWeaponBase::StartBlock()
//{
//	if(WeaponIsIdle())
//	{
//		if (GetLocalRole() < ENetRole::ROLE_Authority)
//		{
//			ServerStartBlock();
//		}
//
//		if (!bIsBlocking)
//		{
//			bIsBlocking = true;
//			TriggerOwnerPlayAttackAnimation(MeleeWeaponData.StartBlockAnims);
//			if (MeleeWeaponData.bCanMagicBlock)
//			{
//				EnableMagicChargeCollisions(true);
//			}
//		}
//	}
//}
//
//
//
//
//void ASMeleeWeaponBase::ServerStartBlock_Implementation()
//{
//	StartBlock();
//}
//
//
//void ASMeleeWeaponBase::StopBlock()
//{
//	if (GetLocalRole() < ENetRole::ROLE_Authority)
//	{
//		ServerStopBlock();
//	}
//
//	if (bIsBlocking)
//	{
//		bIsBlocking = false;
//		TriggerOwnerPlayAttackAnimation(MeleeWeaponData.StopBlockAnims);
//		if (MeleeWeaponData.bCanMagicBlock)
//		{
//			EnableMagicChargeCollisions(false);
//		}
//	}
//}
//
//void ASMeleeWeaponBase::ServerStopBlock_Implementation()
//{
//	StopBlock();
//}
//
//
//
//
//void ASMeleeWeaponBase::OnRep_IsBlocking()
//{
//	if (MyPawn->IsLocallyControlled() == false)
//	{
//		bIsBlocking ? TriggerOwnerPlayAttackAnimation(MeleeWeaponData.StartBlockAnims) : TriggerOwnerPlayAttackAnimation(MeleeWeaponData.StopBlockAnims);
//	}
//}
//
//#pragma endregion
//
//#pragma region Magic Charge
//
//void ASMeleeWeaponBase::OnMagicChargeStateChange(bool IsMagicCharged)
//{
//	if (GetLocalRole() != ENetRole::ROLE_Authority) return;
//	bIsMagicCharged = IsMagicCharged;
//	OnRep_IsMagicCharged();
//}
//
//
//void ASMeleeWeaponBase::EnableMagicChargeCollisions(bool Enable)
//{
//	if (MagicChargeCollisionComp)
//	{
//		Enable ? MagicChargeCollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly) : MagicChargeCollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
//	}
//}
//
//
//void ASMeleeWeaponBase::OnRep_IsMagicCharged()
//{
//	bIsMagicCharged ? ApplyMagicChargeEffects() : RemoveMagicChargeEffects();
//}
//
//
//void ASMeleeWeaponBase::StartUseMagicCharge()
//{
//	if (WeaponIsIdle())
//	{
//		if (GetLocalRole() < ENetRole::ROLE_Authority)
//		{
//			ServerStartUseMagicCharge();
//		}
//
//		if (bIsMagicCharged && ChargeState == EMeleeWeaponChargeState::EMWCS_Idle)
//		{
//			TriggerOwnerPlayAttackAnimation(MeleeWeaponData.MagicChargeAttack.Start);
//			if (GetLocalRole() == ENetRole::ROLE_Authority)
//			{
//				if (PlayAnimMontageOnServer(MeleeWeaponData.MagicChargeAttack.Start.FirstPerson) > ANIM_PLAY_FAIL)
//				{
//					TrySetChargeState(EMeleeWeaponChargeState::EMWCS_Charging);
//				}
//			}
//		}
//	}
//}
//
//
//void ASMeleeWeaponBase::ServerStartUseMagicCharge_Implementation()
//{
//	StartUseMagicCharge();
//}
//
//void ASMeleeWeaponBase::FinishUseMagicCharge()
//{
//	if (GetLocalRole() < ENetRole::ROLE_Authority)
//	{
//		ServerFinishMagicCharge();
//	}
//
//	if (bIsMagicCharged && ChargeState == EMeleeWeaponChargeState::EMWCS_Charged)
//	{
//		TriggerOwnerPlayAttackAnimation(MeleeWeaponData.MagicChargeAttack.Finish);
//		if (GetLocalRole() == ENetRole::ROLE_Authority)
//		{
//			if (PlayAnimMontageOnServer(MeleeWeaponData.MagicChargeAttack.Finish.FirstPerson) > ANIM_PLAY_FAIL)
//			{
//				TrySetChargeState(EMeleeWeaponChargeState::EMWCS_Finish);
//			}
//		}
//	}
//}
//
//void ASMeleeWeaponBase::ServerFinishMagicCharge_Implementation()
//{
//	FinishUseMagicCharge();
//}
//
//void ASMeleeWeaponBase::OnMagicChargeReady()
//{
//	if (GetLocalRole() == ENetRole::ROLE_Authority)
//	{
//		TrySetChargeState(EMeleeWeaponChargeState::EMWCS_Charged);
//	}
//}
//
//void ASMeleeWeaponBase::OnMagicChargeRelease()
//{
//	if (GetLocalRole() == ENetRole::ROLE_Authority)
//	{
//		if (bIsMagicCharged && ChargeState == EMeleeWeaponChargeState::EMWCS_Finish)
//		{
//			TrySetChargeState(EMeleeWeaponChargeState::EMWCS_Idle);
//
//			if (MyPawn && MagicChargeComp && MagicChargeComp->TryRemoveMagicCharge())
//			{
//				ReleaseMagicCharge(MyPawn->GetMagicLaunchTransform());
//			}
//		}
//	}
//}
//
//#pragma endregion
//
//
//
//
//
//
//
//
//
//#pragma region Weapon Setup
//
//void ASMeleeWeaponBase::SetOwningPawn(ASCharacterBase* NewOwner)
//{
//	if (NewOwner != MyPawn)
//	{
//		SetInstigator(NewOwner);
//		MyPawn = NewOwner;
//		SetOwner(NewOwner);
//	}
//}
//
//
//void ASMeleeWeaponBase::OnRep_MyPawn()
//{
//	if (MyPawn)
//	{
//		OnEnterInventory(MyPawn);
//	}
//	else
//	{
//		OnLeaveInventory();
//	}
//}
//
//
//void ASMeleeWeaponBase::OnEnterInventory(ASCharacterBase* NewOwner)
//{
//	SetOwningPawn(NewOwner);
//}
//
//
//void ASMeleeWeaponBase::OnLeaveInventory()
//{
//
//}
//
//
//void ASMeleeWeaponBase::OnEquip()
//{
//	AttachMeshToPawn();
//}
//
//
//void ASMeleeWeaponBase::AttachMeshToPawn()
//{
//	if (!MyPawn) return;
//
//	GetRootComponent()->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
//	MeshComp->SetHiddenInGame(true);
//
//	FName AttackSocketName = MyPawn->GetWeaponSocketName();
//	if (MyPawn->IsLocallyControlled())
//	{
//		if (GetLocalRole() != ENetRole::ROLE_Authority)
//		{
//			UE_LOG(LogTemp, Warning, TEXT("AttachMeshToPawn, not server"));
//		}
//		auto MyPawnFPP = MyPawn->GetFirstPersonMesh();
//		if (MyPawnFPP)
//		{
//			MeshComp->SetHiddenInGame(false);
//			GetRootComponent()->AttachToComponent(MyPawnFPP, FAttachmentTransformRules::KeepRelativeTransform, AttackSocketName);
//
//		}
//	}
//	else
//	{
//		auto MyPawnTPP = MyPawn->GetThirdPersonMesh();
//		if (MyPawnTPP)
//		{
//			MeshComp->SetHiddenInGame(false);
//			GetRootComponent()->AttachToComponent(MyPawnTPP, FAttachmentTransformRules::KeepRelativeTransform, AttackSocketName);
//		}
//	}
//}
//
//#pragma endregion
//
//void ASMeleeWeaponBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//	DOREPLIFETIME(ASMeleeWeaponBase, MyPawn);
//	//DOREPLIFETIME_CONDITION_NOTIFY(ASMeleeWeaponBase, MeleeAttackCount, COND_Custom, REPNOTIFY_Always);
//	DOREPLIFETIME(ASMeleeWeaponBase, MeleeAttackCount);
//	
//	DOREPLIFETIME(ASMeleeWeaponBase, UseState);
//	DOREPLIFETIME(ASMeleeWeaponBase, ChargeState);
//
//	DOREPLIFETIME(ASMeleeWeaponBase, bIsBlocking);
//	DOREPLIFETIME(ASMeleeWeaponBase, bIsMagicCharged);
//	
//	
//
//
//}