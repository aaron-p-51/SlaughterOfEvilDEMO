// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SCharacterBase.h"

// Engine Includes
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


// Game Includes
#include "Weapons/SMeleeWeaponBase.h"

const static float MONTAGE_PLAY_FAIL = 0.f;
const static int32 NO_WEAPON_SET = -1;

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

	
	
	CurrentWeaponIndex = NO_WEAPON_SET;
	MagicUseState = EMagicUseState::EMUS_Idle;
	MeleeAttackState = EMeleeAttackState::EMAS_Idle;
	
	bReplicates = true;
	SetReplicateMovement(true);

}


// Called when the game starts or when spawned
void ASCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	SpawnStartingWeapons();
	
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


void ASCharacterBase::SpawnStartingWeapons()
{

	for (int32 i = 0; i < MeleeWeaponData.Num(); ++i)
	{
		// TPP Weapon
		FTransform WeaponSpawnTransformTPP(GetActorTransform());
		MeleeWeaponData[i].TPPMeleeWeapon = Cast<ASMeleeWeaponBase>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, MeleeWeaponData[i].MeleeWeaponClass, WeaponSpawnTransformTPP));
		if (MeleeWeaponData[i].TPPMeleeWeapon && GetMesh())
		{
			MeleeWeaponData[i].TPPMeleeWeapon->SetOwner(this);
			MeleeWeaponData[i].TPPMeleeWeapon->SetInstigator(this);
			MeleeWeaponData[i].TPPMeleeWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, MeleeWeaponData[i].MeleeWeaponSocketName);

			// Hide weapon, TPP weapon only shown for simulated proxies
			if (IsLocallyControlled())
			{
				MeleeWeaponData[i].TPPMeleeWeapon->SetWeaponVisibility(false);
			}
			UGameplayStatics::FinishSpawningActor(MeleeWeaponData[i].TPPMeleeWeapon, WeaponSpawnTransformTPP);
		}

		// TPP Weapon
		FTransform WeaponSpawnTransformFPP(GetActorTransform());
		MeleeWeaponData[i].FPPMeleeWeapon = Cast<ASMeleeWeaponBase>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, MeleeWeaponData[i].MeleeWeaponClass, WeaponSpawnTransformFPP));
		if (MeleeWeaponData[i].FPPMeleeWeapon && FirstPersonMesh)
		{
			MeleeWeaponData[i].FPPMeleeWeapon->SetOwner(this);
			MeleeWeaponData[i].FPPMeleeWeapon->SetInstigator(this);
			MeleeWeaponData[i].FPPMeleeWeapon->AttachToComponent(FirstPersonMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, MeleeWeaponData[i].MeleeWeaponSocketName);
			
			// Hide weapon, FPP weapon only shown for autonomous proxies and actors with authority
			if (!IsLocallyControlled())
			{
				MeleeWeaponData[i].FPPMeleeWeapon->SetWeaponVisibility(false);
			}
			UGameplayStatics::FinishSpawningActor(MeleeWeaponData[i].FPPMeleeWeapon, WeaponSpawnTransformFPP);
		}
	}

	// Set the current weapon index based on which weapon has bIsStartingWeapon set.
	for (int32 i = 0; i < MeleeWeaponData.Num(); ++i)
	{
		if (MeleeWeaponData[i].bIsStartingWeapon && CurrentWeaponIndex == NO_WEAPON_SET)
		{
			CurrentWeaponIndex = i;
		}
		else if (MeleeWeaponData[i].bIsStartingWeapon && CurrentWeaponIndex > NO_WEAPON_SET)
		{
			UE_LOG(LogTemp, Warning, TEXT("Multiple starting weapons set for %s. Starting weapon will be set to %s"), *GetName(), *MeleeWeaponData[i].MeleeWeaponClass->GetName());
		}
	}
	
}


// Called every frame
void ASCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsLocallyControlled() == false && Camera != nullptr)
	{
		FRotator NewRot = Camera->GetRelativeRotation();
		NewRot.Pitch = RemoteViewPitch * 360.f / 255.f;

		Camera->SetRelativeRotation(NewRot);
	}
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
	if (MeleeAttackState == EMeleeAttackState::EMAS_Idle)
	{
		TrySetMeleeAttackState(EMeleeAttackState::EMAS_Attacking);
		MeleeAttackDirection = GetMeleeAttackDirection();

		auto ServerFirstPersonAttackMontage = MeleeWeaponData[CurrentWeaponIndex].FPPAttackMontages.Find(MeleeAttackDirection);
		if (*ServerFirstPersonAttackMontage && FirstPersonAnimInstance)
		{
			FirstPersonAnimInstance->Montage_Play(*ServerFirstPersonAttackMontage);
			MulticastPlayMeleeAttackMontage(MeleeAttackDirection);
		}
	}
	
}


// TODO: Replace with deterministic function, get rid of randomness
EMeleeAttackDirection ASCharacterBase::GetMeleeAttackDirection() const
{
	int32 RandomSwingIndex = UKismetMathLibrary::RandomIntegerInRange(0, 1);
	
	switch (MeleeAttackDirection)
	{
		case EMeleeAttackDirection::EMAD_Left:
		case EMeleeAttackDirection::EMAD_LeftDown:
			
			if (RandomSwingIndex == 0)
			{
				return EMeleeAttackDirection::EMAD_Right;
			}
			else
			{
				return EMeleeAttackDirection::EMAD_RightDown;
			}
		break;

		case EMeleeAttackDirection::EMAD_Right:
		case EMeleeAttackDirection::EMAD_RightDown:

			if (RandomSwingIndex == 0)
			{
				return EMeleeAttackDirection::EMAD_Left;
			}
			else
			{
				return EMeleeAttackDirection::EMAD_LeftDown;
			}
		break;

	default:
		return MeleeAttackDirection;
		break;
	}
}


void ASCharacterBase::MulticastPlayMeleeAttackMontage_Implementation(EMeleeAttackDirection MeleeAttack)
{
	PlayMontagePairTPPandFPP(*(MeleeWeaponData[CurrentWeaponIndex].FPPAttackMontages.Find(MeleeAttack)),
		*(MeleeWeaponData[CurrentWeaponIndex].TPPAttackMontages.Find(MeleeAttack)));
}


bool ASCharacterBase::TrySetMeleeAttackState(EMeleeAttackState NewMeleeAttackState)
{
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		MeleeAttackState = NewMeleeAttackState;

		if (MeleeWeaponData[CurrentWeaponIndex].FPPMeleeWeapon)
		{
			if (MeleeAttackState == EMeleeAttackState::EMAS_CauseDamage)
			{
				MeleeWeaponData[CurrentWeaponIndex].FPPMeleeWeapon->SetCanCauseDamage(true);
			}
			else
			{
				MeleeWeaponData[CurrentWeaponIndex].FPPMeleeWeapon->SetCanCauseDamage(false);
			}
		}
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
	if (MeleeWeaponData[CurrentWeaponIndex].FPPMeleeWeapon)
	{
		MeleeWeaponData[CurrentWeaponIndex].FPPMeleeWeapon->SetIsBlocking(IsBlocking);
	}
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
		
		if (MagicUseState == EMagicUseState::EMUS_Start && FirstPersonAnimInstance && MeleeWeaponData[CurrentWeaponIndex].FPPMagicStartMontage)
		{
			// FirstFPP anim montage contains notifies to progress through MagicUseState
			FirstPersonAnimInstance->Montage_Play(MeleeWeaponData[CurrentWeaponIndex].FPPMagicStartMontage);
			MulticastPlayStarUsetWeaponMagic();
		}
	}	
}


void ASCharacterBase::MulticastPlayStarUsetWeaponMagic_Implementation()
{
	// Play corresponding animations for non authority for FPP and TPP 
	PlayMontagePairTPPandFPP(MeleeWeaponData[CurrentWeaponIndex].FPPMagicStartMontage,
		MeleeWeaponData[CurrentWeaponIndex].FPPMagicStartMontage);
}


void ASCharacterBase::FinishUseWeaponMagic()
{
	if (GetLocalRole() < ENetRole::ROLE_Authority)
	{
		ServerFinishUseWeaponMagic();
	}
	
	// If trying to finish using melee weapon magic before it is ready. Stop FPPMagicStartMontage
	if (MagicUseState != EMagicUseState::EMUS_Ready)
	{
		FirstPersonAnimInstance->Montage_Stop(0.35f, MeleeWeaponData[CurrentWeaponIndex].FPPMagicStartMontage);
		return;
	}

	if (GetLocalRole() == ENetRole::ROLE_Authority && IsCurrentMeleeWeaponMagicCharged() &&
		MagicUseState == EMagicUseState::EMUS_Ready && FirstPersonAnimInstance && MeleeWeaponData[CurrentWeaponIndex].FPPMagicFinishMontage)
	{
		// FirstFPP anim montage contains notifies to progress through MagicUseState
		FirstPersonAnimInstance->Montage_Play(MeleeWeaponData[CurrentWeaponIndex].FPPMagicFinishMontage);
		MulticastPlayFinishUseWeaponMagic();
	}
}


void ASCharacterBase::ServerFinishUseWeaponMagic_Implementation()
{
	FinishUseWeaponMagic();
}


void ASCharacterBase::MulticastPlayFinishUseWeaponMagic_Implementation()
{	
	// Play corresponding animations for non authority for FPP and TPP 
	PlayMontagePairTPPandFPP(MeleeWeaponData[CurrentWeaponIndex].FPPMagicFinishMontage,
		MeleeWeaponData[CurrentWeaponIndex].TPPMagicFinishMontage);
}


void ASCharacterBase::ReleaseWeaponMagic()
{
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		// Camera transform is used for release point of magic
		if (Camera)
		{
			FTransform ProjectileSpawnTransform = Camera->GetComponentTransform();
			MeleeWeaponData[CurrentWeaponIndex].FPPMeleeWeapon->ReleaseMagicCharge(ProjectileSpawnTransform);
		}

		TrySetMagicUseState(EMagicUseState::EMUS_Idle);
	}
}


bool ASCharacterBase::TrySetMagicUseState(EMagicUseState NewMagicUseState)
{
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		if (!MeleeWeaponData[CurrentWeaponIndex].FPPMeleeWeapon) return false;
		bool FPPWeaponMagicCharged = MeleeWeaponData[CurrentWeaponIndex].FPPMeleeWeapon->GetIsWeaponMagicCharged();

		switch (NewMagicUseState)
		{
		case EMagicUseState::EMUS_NoCharge:
			if (!FPPWeaponMagicCharged)
			{
				MagicUseState = EMagicUseState::EMUS_NoCharge;
				return true;
			}
			break;
		case EMagicUseState::EMUS_Idle:
			if (FPPWeaponMagicCharged)
			{
				MagicUseState = EMagicUseState::EMUS_Idle;
				return true;
			}
			break;
		case EMagicUseState::EMUS_Start:
			if (FPPWeaponMagicCharged && MagicUseState == EMagicUseState::EMUS_Idle)
			{
				MagicUseState = EMagicUseState::EMUS_Start;
				return true;
			}
			break;
		case EMagicUseState::EMUS_Ready:
			if (FPPWeaponMagicCharged && MagicUseState == EMagicUseState::EMUS_Start)
			{
				MagicUseState = EMagicUseState::EMUS_Ready;
				return true;
			}
			break;
		case EMagicUseState::EMUS_Finish:
			if (FPPWeaponMagicCharged && MagicUseState == EMagicUseState::EMUS_Ready)
			{
				MagicUseState = EMagicUseState::EMUS_Finish;
				ReleaseWeaponMagic();
				return true;
			}
			break;
		default:
			return false;
			//break;
		}
	}

	return false;
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

	// Play animation to show impact, will only play for FPP
	if (bCurrentWeaponIsMagicCharged && IsLocallyControlled() &&
		FirstPersonAnimInstance && MeleeWeaponData[CurrentWeaponIndex].BlockImpactMontage)
	{
		FirstPersonAnimInstance->Montage_Play(MeleeWeaponData[CurrentWeaponIndex].BlockImpactMontage);
	}

	// Apply/Remove magic charge effects on TPPweapon
	if (MeleeWeaponData[CurrentWeaponIndex].TPPMeleeWeapon)
	{
		bCurrentWeaponIsMagicCharged ? MeleeWeaponData[CurrentWeaponIndex].TPPMeleeWeapon->ApplyMagicChargeEffects() :
			MeleeWeaponData[CurrentWeaponIndex].TPPMeleeWeapon->RemoveMagicChargeEffects();
	}

	// Apply/Remove magic charge effects on FPPweapon
	if (MeleeWeaponData[CurrentWeaponIndex].FPPMeleeWeapon)
	{
		bCurrentWeaponIsMagicCharged ? MeleeWeaponData[CurrentWeaponIndex].FPPMeleeWeapon->ApplyMagicChargeEffects() :
			MeleeWeaponData[CurrentWeaponIndex].FPPMeleeWeapon->RemoveMagicChargeEffects();
	}
}


/*************************************************************************/
/* Helpers  */
/*************************************************************************/

bool ASCharacterBase::IsCurrentMeleeWeaponMagicCharged() const
{
	return (MeleeWeaponData[CurrentWeaponIndex].FPPMeleeWeapon && MeleeWeaponData[CurrentWeaponIndex].FPPMeleeWeapon->GetIsWeaponMagicCharged());
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


void ASCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASCharacterBase, bIsBlocking);
	DOREPLIFETIME(ASCharacterBase, bCurrentWeaponIsMagicCharged);
	DOREPLIFETIME(ASCharacterBase, MagicUseState);
}

