 //Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SCharacterBase.h"

// Engine Includes
#include "AIController.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"



// Game Includes
#include "Weapons/SMeleeWeaponBase.h"

const static float MONTAGE_PLAY_FAIL = 0.f;


// Sets default values
ASCharacterBase::ASCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	if (Camera)
	{
		Camera->SetupAttachment(GetRootComponent());
		Camera->bUsePawnControlRotation = true;
	}

	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	if (FirstPersonMesh && Camera)
	{
		FirstPersonMesh->SetupAttachment(Camera);
		FirstPersonMesh->SetOnlyOwnerSee(true);
		FirstPersonMesh->SetOwnerNoSee(false);
	}

	auto ThirdPersonMesh = GetMesh();
	if (ThirdPersonMesh)
	{
		ThirdPersonMesh->SetOnlyOwnerSee(false);
		ThirdPersonMesh->SetOwnerNoSee(true);
	}

	ProjectileHomingTarget = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileHomingTarget"));
	if (ProjectileHomingTarget)
	{
		ProjectileHomingTarget->SetupAttachment(GetRootComponent());
	}

	
	
	
	MagicUseState = EMagicUseState::EMUS_Idle;
	MeleeAttackState = EMeleeAttackState::EMAS_Idle;
	
	bReplicates = true;
	SetReplicateMovement(true);

}

void ASCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		// Needs to happen after character is added to repgraph (from shooter demo)
		GetWorldTimerManager().SetTimerForNextTick(this, &ASCharacterBase::SpawnDefaultInventory);
	}
}

// Called when the game starts or when spawned
void ASCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	SetProjectileHomingTargetLocation(false);
	
	// Cache the Animation Instance for first person mesh. DO NOT place this in
	// constructor, will not be initialized properly
	if (FirstPersonMesh)
	{
		FirstPersonAnimInstance = FirstPersonMesh->GetAnimInstance();
	}
	if (GetMesh())
	{
		ThirdPersonAnimInstance = GetMesh()->GetAnimInstance();
	}
}


void ASCharacterBase::OnRep_CurrentMeleeWeapon(ASMeleeWeaponBase* LastWeapon)
{
	SetCurrentWeapon(CurrentMeleeWeapon, LastWeapon);
}








// Called every frame
void ASCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsLocallyControlled() == false && Camera)
	{
		FRotator NewRot = Camera->GetRelativeRotation();
		NewRot.Pitch = RemoteViewPitch * 360.f / 255.f;

		Camera->SetRelativeRotation(NewRot);
	}
}


bool ASCharacterBase::IsFirstPerson() const
{
	return Controller && Controller->IsLocalPlayerController();
}

FRotator ASCharacterBase::GetRemotePitchView() const
{
	return Camera ? Camera->GetRelativeRotation() : FRotator::ZeroRotator;
}

// Called to bind functionality to input
void ASCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UE_LOG(LogTemp, Warning, TEXT("SetupPlayerInputComponent()"));

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ASCharacterBase::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ASCharacterBase::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &ASCharacterBase::MeleeAttack);

	PlayerInputComponent->BindAction(TEXT("Block"), EInputEvent::IE_Pressed, this, &ASCharacterBase::MeleeBlockStart);
	PlayerInputComponent->BindAction(TEXT("Block"), EInputEvent::IE_Released, this, &ASCharacterBase::MeleeBlockStop);

	PlayerInputComponent->BindAction(TEXT("WeaponMagic"), EInputEvent::IE_Pressed, this, &ASCharacterBase::StartUseWeaponMagic);
	PlayerInputComponent->BindAction(TEXT("WeaponMagic"), EInputEvent::IE_Released, this, &ASCharacterBase::FinishUseWeaponMagic);


}


void ASCharacterBase::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector() * Value);	
}


void ASCharacterBase::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector() * Value);
}


/*************************************************************************/
/* Melee Weapon Attack */
/*************************************************************************/
void ASCharacterBase::MeleeAttack()
{
	if (GetLocalRole() < ENetRole::ROLE_Authority)
	{
		ServerTryMeleeAttack();
	}
	else
	{
		ServerTryMeleeAttack_Implementation();
	}
}


void ASCharacterBase::ServerTryMeleeAttack_Implementation()
{
	/*if (MeleeAttackState == EMeleeAttackState::EMAS_Idle)
	{
		TrySetMeleeAttackState(EMeleeAttackState::EMAS_Attacking);

		const int32 RandomMeleeAttackIndex = FMath::RandRange(0, MeleeWeaponData[CurrentWeaponIndex].MeleeAttacks.Num() - 1);
		
		if (!IsAIControlled())
		{
			UAnimMontage* ServerFirstPersonAttackMontage = MeleeWeaponData[CurrentWeaponIndex].MeleeAttacks[RandomMeleeAttackIndex].FPPMeleeAttack;
			if (ServerFirstPersonAttackMontage && FirstPersonAnimInstance)
			{
				FirstPersonAnimInstance->Montage_Play(ServerFirstPersonAttackMontage);
			}
		}

		MulticastPlayMeleeAttackMontage(RandomMeleeAttackIndex);			
	}*/
}


void ASCharacterBase::MulticastPlayMeleeAttackMontage_Implementation(int32 MeleeAttackIndex)
{

	/*if (MeleeAttackIndex < MeleeWeaponData[CurrentWeaponIndex].MeleeAttacks.Num())
	{
		if (IsAIControlled() && ThirdPersonAnimInstance)
		{
			ThirdPersonAnimInstance->Montage_Play(MeleeWeaponData[CurrentWeaponIndex].MeleeAttacks[MeleeAttackIndex].TPPMeleeAttack);
		}
		else
		{
			PlayMontagePairTPPandFPP((MeleeWeaponData[CurrentWeaponIndex].MeleeAttacks[MeleeAttackIndex].FPPMeleeAttack),
				(MeleeWeaponData[CurrentWeaponIndex].MeleeAttacks[MeleeAttackIndex].TPPMeleeAttack));
		}
	}*/
}


void ASCharacterBase::MeleeAttackCanCauseDamage(bool Value)
{
	if (Value)
	{
		TrySetMeleeAttackState(EMeleeAttackState::EMAS_CauseDamage);
	}
	else
	{
		TrySetMeleeAttackState(EMeleeAttackState::EMAS_Attacking);
	}
}


void ASCharacterBase::MeleeAttackFinished()
{
	TrySetMeleeAttackState(EMeleeAttackState::EMAS_Idle);
}


bool ASCharacterBase::TrySetMeleeAttackState(EMeleeAttackState NewMeleeAttackState)
{
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		MeleeAttackState = NewMeleeAttackState;

		/*if (MeleeWeaponData[CurrentWeaponIndex].FPPMeleeWeapon)
		{
			if (MeleeAttackState == EMeleeAttackState::EMAS_CauseDamage)
			{
				MeleeWeaponData[CurrentWeaponIndex].FPPMeleeWeapon->SetCanCauseDamage(true);
			}
			else
			{
				MeleeWeaponData[CurrentWeaponIndex].FPPMeleeWeapon->SetCanCauseDamage(false);
			}
		}*/
	}

	return true;
}

#pragma region MELEE_WEAPON_BLOCK
/*************************************************************************/
/* Melee Weapon Block */
/*************************************************************************/

void ASCharacterBase::MeleeBlockStart()
{
	if (GetLocalRole() < ENetRole::ROLE_Authority)
	{
		ServerTrySetWeaponBlocking(true);
	}
	else
	{
		ServerTrySetWeaponBlocking_Implementation(true);
	}
}


void ASCharacterBase::MeleeBlockStop()
{
	if (GetLocalRole() < ENetRole::ROLE_Authority)
	{
		ServerTrySetWeaponBlocking(false);
	}
	else
	{
		ServerTrySetWeaponBlocking_Implementation(false);
	}
}


void ASCharacterBase::ServerTrySetWeaponBlocking_Implementation(bool IsBlocking)
{
	bIsBlocking = IsBlocking;
	/*if (MeleeWeaponData[CurrentWeaponIndex].FPPMeleeWeapon)
	{
		MeleeWeaponData[CurrentWeaponIndex].FPPMeleeWeapon->SetIsBlocking(IsBlocking);
	}*/
}


bool ASCharacterBase::IsBlocking()
{
	return bIsBlocking;
}

#pragma endregion


/*************************************************************************/
/* Weapon magic  */
/*************************************************************************/

void ASCharacterBase::StartUseWeaponMagic()
{
	(GetLocalRole() < ENetRole::ROLE_Authority) ? ServerStartUseWeaponMagic() : ServerStartUseWeaponMagic_Implementation();

}


void ASCharacterBase::ServerStartUseWeaponMagic_Implementation()
{
	if (IsCurrentMeleeWeaponMagicCharged())
	{
		// Try and set Magic use state to start using magic
		TrySetMagicUseState(EMagicUseState::EMUS_Start);
		
		//if (MagicUseState == EMagicUseState::EMUS_Start && FirstPersonAnimInstance && MeleeWeaponData[CurrentWeaponIndex].FPPMagicStartMontage)
		//{
		//	// FirstFPP anim montage contains notifies to progress through MagicUseState
		//	FirstPersonAnimInstance->Montage_Play(MeleeWeaponData[CurrentWeaponIndex].FPPMagicStartMontage);
		//	MulticastPlayStarUsetWeaponMagic();
		//}
	}	
}


void ASCharacterBase::MulticastPlayStarUsetWeaponMagic_Implementation()
{
	// Play corresponding animations for non authority for FPP and TPP 
	/*PlayMontagePairTPPandFPP(MeleeWeaponData[CurrentWeaponIndex].FPPMagicStartMontage,
		MeleeWeaponData[CurrentWeaponIndex].FPPMagicStartMontage);*/
}


void ASCharacterBase::FinishUseWeaponMagic()
{
	//if (GetLocalRole() < ENetRole::ROLE_Authority)
	//{
	//	ServerFinishUseWeaponMagic();
	//}
	//
	//// If trying to finish using melee weapon magic before it is ready. Stop FPPMagicStartMontage
	//if (MagicUseState != EMagicUseState::EMUS_Ready)
	//{
	//	//FirstPersonAnimInstance->Montage_Stop(0.35f, MeleeWeaponData[CurrentWeaponIndex].FPPMagicStartMontage);
	//	return;
	//}

	//if (GetLocalRole() == ENetRole::ROLE_Authority && IsCurrentMeleeWeaponMagicCharged() &&
	//	//MagicUseState == EMagicUseState::EMUS_Ready && FirstPersonAnimInstance && MeleeWeaponData[CurrentWeaponIndex].FPPMagicFinishMontage)
	//{
	//	// FirstFPP anim montage contains notifies to progress through MagicUseState
	//	FirstPersonAnimInstance->Montage_Play(MeleeWeaponData[CurrentWeaponIndex].FPPMagicFinishMontage);
	//	MulticastPlayFinishUseWeaponMagic();
	//}
}


void ASCharacterBase::ServerFinishUseWeaponMagic_Implementation()
{
	FinishUseWeaponMagic();
}


void ASCharacterBase::MulticastPlayFinishUseWeaponMagic_Implementation()
{	
	// Play corresponding animations for non authority for FPP and TPP 
	/*PlayMontagePairTPPandFPP(MeleeWeaponData[CurrentWeaponIndex].FPPMagicFinishMontage,
		MeleeWeaponData[CurrentWeaponIndex].TPPMagicFinishMontage);*/
}


void ASCharacterBase::ReleaseWeaponMagic()
{
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		// Camera transform is used for release point of magic
		if (Camera)
		{
			FTransform ProjectileSpawnTransform = Camera->GetComponentTransform();
			//MeleeWeaponData[CurrentWeaponIndex].FPPMeleeWeapon->ReleaseMagicCharge(ProjectileSpawnTransform);
		}

		TrySetMagicUseState(EMagicUseState::EMUS_Idle);
	}
}


void ASCharacterBase::ProjectileMagicAttack()
{
	
}


void ASCharacterBase::MulticastPlayProjectileMagicAttackMontage_Implementation(int32 AttackIndex)
{
	if (AttackIndex < ProjectileMagicCasts.Num())
	{
		if (IsAIControlled() && ThirdPersonAnimInstance)
		{
			ThirdPersonAnimInstance->Montage_Play(ProjectileMagicCasts[AttackIndex].TPPMagicCastAnimMontage);
		}
		else
		{
			PlayMontagePairTPPandFPP(ProjectileMagicCasts[AttackIndex].FPPMagicCastAnimMontage, ProjectileMagicCasts[AttackIndex].TPPMagicCastAnimMontage);
		}
	}
}


void ASCharacterBase::SpawnMagicAttackProjectile()
{


}


bool ASCharacterBase::TrySetMagicUseState(EMagicUseState NewMagicUseState)
{
	//if (GetLocalRole() == ENetRole::ROLE_Authority)
	//{
	//	//if (!MeleeWeaponData[CurrentWeaponIndex].FPPMeleeWeapon) return false;
	//	bool FPPWeaponMagicCharged = MeleeWeaponData[CurrentWeaponIndex].FPPMeleeWeapon->GetIsWeaponMagicCharged();

	//	switch (NewMagicUseState)
	//	{
	//	case EMagicUseState::EMUS_NoCharge:
	//		if (!FPPWeaponMagicCharged)
	//		{
	//			MagicUseState = EMagicUseState::EMUS_NoCharge;
	//			return true;
	//		}
	//		break;
	//	case EMagicUseState::EMUS_Idle:
	//		if (FPPWeaponMagicCharged)
	//		{
	//			MagicUseState = EMagicUseState::EMUS_Idle;
	//			return true;
	//		}
	//		break;
	//	case EMagicUseState::EMUS_Start:
	//		if (FPPWeaponMagicCharged && MagicUseState == EMagicUseState::EMUS_Idle)
	//		{
	//			MagicUseState = EMagicUseState::EMUS_Start;
	//			return true;
	//		}
	//		break;
	//	case EMagicUseState::EMUS_Ready:
	//		if (FPPWeaponMagicCharged && MagicUseState == EMagicUseState::EMUS_Start)
	//		{
	//			MagicUseState = EMagicUseState::EMUS_Ready;
	//			return true;
	//		}
	//		break;
	//	case EMagicUseState::EMUS_Finish:
	//		if (FPPWeaponMagicCharged && MagicUseState == EMagicUseState::EMUS_Ready)
	//		{
	//			MagicUseState = EMagicUseState::EMUS_Finish;
	//			ReleaseWeaponMagic();
	//			return true;
	//		}
	//		break;
	//	default:
	//		return false;
	//		break;
	//	}
	//}

	return false;
}


TWeakObjectPtr<USceneComponent> ASCharacterBase::GetProjectileHomingTarget()
{
	return TWeakObjectPtr<USceneComponent>(ProjectileHomingTarget);
}

void ASCharacterBase::WeaponMagicChargeChange(bool Value)
{
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		// Not magic charged
		if (!Value)
		{
			bool success = TrySetMagicUseState(EMagicUseState::EMUS_NoCharge);
#if WITH_EDITOR
			if (!success) UE_LOG(LogTemp, Warning, TEXT("Player: %s, is no longer has magic charged weapon, fail to set MagicUseState to EMUS_NoCharge"), *GetName());
#endif
		}
		else
		{
			bool success = TrySetMagicUseState(EMagicUseState::EMUS_Idle);
#if WITH_EDITOR
			if (!success) UE_LOG(LogTemp, Warning, TEXT("Player: %s, has magic charged weapon, failed to set MagicUseState to EMUS_Idle"), *GetName());
#endif
		}

		bCurrentWeaponIsMagicCharged = Value;
		OnRep_CurrentWeaponIsMagicCharged();
	}
}




void ASCharacterBase::OnRep_CurrentWeaponIsMagicCharged()
{

	//// Play animation to show impact, will only play for FPP
	//if (bCurrentWeaponIsMagicCharged && IsLocallyControlled() &&
	//	FirstPersonAnimInstance && MeleeWeaponData[CurrentWeaponIndex].BlockImpactMontage)
	//{
	//	FirstPersonAnimInstance->Montage_Play(MeleeWeaponData[CurrentWeaponIndex].BlockImpactMontage);
	//}

	//// Apply/Remove magic charge effects on TPPweapon
	//if (MeleeWeaponData[CurrentWeaponIndex].TPPMeleeWeapon)
	//{
	//	bCurrentWeaponIsMagicCharged ? MeleeWeaponData[CurrentWeaponIndex].TPPMeleeWeapon->ApplyMagicChargeEffects() :
	//		MeleeWeaponData[CurrentWeaponIndex].TPPMeleeWeapon->RemoveMagicChargeEffects();
	//}

	//// Apply/Remove magic charge effects on FPPweapon
	//if (MeleeWeaponData[CurrentWeaponIndex].FPPMeleeWeapon)
	//{
	//	bCurrentWeaponIsMagicCharged ? MeleeWeaponData[CurrentWeaponIndex].FPPMeleeWeapon->ApplyMagicChargeEffects() :
	//		MeleeWeaponData[CurrentWeaponIndex].FPPMeleeWeapon->RemoveMagicChargeEffects();
	//}
}


/*************************************************************************/
/* Helpers  */
/*************************************************************************/

bool ASCharacterBase::IsCurrentMeleeWeaponMagicCharged() const
{
	//return (MeleeWeaponData[CurrentWeaponIndex].FPPMeleeWeapon && MeleeWeaponData[CurrentWeaponIndex].FPPMeleeWeapon->GetIsWeaponMagicCharged());
	return false;
}


bool ASCharacterBase::IsAIControlled() const
{
	return (Cast<APlayerController>(GetController())) ? false : true;
}

void ASCharacterBase::PlayMontagePairTPPandFPP(UAnimMontage* FPPMontage, UAnimMontage* TPPMontage) const
{
	if (GetLocalRole() < ENetRole::ROLE_Authority && IsLocallyControlled() && FirstPersonAnimInstance && FPPMontage)
	{
		FirstPersonAnimInstance->Montage_Play(FPPMontage);
	}
	else if (!IsLocallyControlled() && ThirdPersonAnimInstance && TPPMontage)
	{
		ThirdPersonAnimInstance->Montage_Play(TPPMontage);
	}
}


void ASCharacterBase::SetProjectileHomingTargetLocation(bool IsCrouching)
{
	if (!ProjectileHomingTarget) return;

	float HeightOffset = (IsCrouching) ? CrouchedEyeHeight : BaseEyeHeight;
	ProjectileHomingTarget->AddLocalOffset(FVector(0.f, 0.f, HeightOffset));
}

void ASCharacterBase::SpawnDefaultInventory()
{
	// Weapons are set to replicate so only spawn on server
	if (GetLocalRole() < ENetRole::ROLE_Authority)
	{
		return;
	}

	MeleeWeaponInventory.Empty();
	int32 MeleeWeaponClasses = MeleeWeaponInventoryClasses.Num();
	for (int32 i = 0; i < MeleeWeaponClasses; ++i)
	{
		if (MeleeWeaponInventoryClasses[i])
		{
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			ASMeleeWeaponBase* NewWeapon = GetWorld()->SpawnActor<ASMeleeWeaponBase>(MeleeWeaponInventoryClasses[i], SpawnParameters);
			AddWeapon(NewWeapon);
		}
	}

	if (MeleeWeaponInventory.Num() > 0)
	{
		EquipWeapon(MeleeWeaponInventory[0]);
	}
}

void ASCharacterBase::AddWeapon(ASMeleeWeaponBase* Weapon)
{
	if (Weapon && GetLocalRole() == ENetRole::ROLE_Authority)
	{
		Weapon->OnEnterInventory(this);
		MeleeWeaponInventory.AddUnique(Weapon);
	}
}

void ASCharacterBase::EquipWeapon(ASMeleeWeaponBase* Weapon)
{
	if (Weapon)
	{
		if (GetLocalRole() < ENetRole::ROLE_Authority)
		{
			ServerEquipWeapon(Weapon);
		}
		else
		{
			SetCurrentWeapon(Weapon, CurrentMeleeWeapon);
		}
	}
}




void ASCharacterBase::SetCurrentWeapon(ASMeleeWeaponBase* NewWeapon, ASMeleeWeaponBase* PreviousWeapon)
{
	ASMeleeWeaponBase* LocalPreviousWeapon = nullptr;

	if (PreviousWeapon != nullptr)
	{
		LocalPreviousWeapon = PreviousWeapon;
	}
	else if (NewWeapon != PreviousWeapon)
	{
		LocalPreviousWeapon = CurrentMeleeWeapon;
	}

	// TODO unequip previous weapon


	CurrentMeleeWeapon = NewWeapon;

	if (NewWeapon)
	{
		NewWeapon->SetOwningPawn(this);
		NewWeapon->OnEquip();
	}
}

void ASCharacterBase::Destroyed()
{
	Super::Destroyed();

	if (GetLocalRole() < ROLE_Authority)
	{
		return;
	}

	// remove all weapons from inventory and destroy them
	for (int32 i = MeleeWeaponInventory.Num() - 1; i >= 0; i--)
	{
		ASMeleeWeaponBase* Weapon = MeleeWeaponInventory[i];
		if (Weapon)
		{
			//RemoveWeapon(Weapon);
			MeleeWeaponInventory.RemoveSingle(Weapon);
		
		}
	}
}

void ASCharacterBase::ServerEquipWeapon_Implementation(ASMeleeWeaponBase* Weapon)
{
	EquipWeapon(Weapon);
}

void ASCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASCharacterBase, bIsBlocking);
	DOREPLIFETIME(ASCharacterBase, bCurrentWeaponIsMagicCharged);
	DOREPLIFETIME(ASCharacterBase, MagicUseState);
	DOREPLIFETIME(ASCharacterBase, CurrentMeleeWeapon);

	// only to local owner: weapon change requests are locally instigated, other clients don't need it
	DOREPLIFETIME_CONDITION(ASCharacterBase, MeleeWeaponInventory, COND_OwnerOnly);
	

}

