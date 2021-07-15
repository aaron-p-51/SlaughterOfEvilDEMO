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

	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &ASCharacterBase::WeaponAttack);
	PlayerInputComponent->BindAction(TEXT("Block"), EInputEvent::IE_Pressed, this, &ASCharacterBase::WeaponBlockStart);
	PlayerInputComponent->BindAction(TEXT("Block"), EInputEvent::IE_Released, this, &ASCharacterBase::WeaponBlockStop);
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
void ASCharacterBase::WeaponAttack()
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
	MeleeAttackDirection = GetMeleeAttackDirection();
	MulticastPlayMeleeAttackMontage(MeleeAttackDirection);
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
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		// Damage window is only set from FPP on server, damage can only be applied 
		auto ServerFirstPersonAttackMontage = MeleeWeaponData[CurrentWeaponIndex].FirstPeronAttackMontages.Find(MeleeAttack);
		if (ServerFirstPersonAttackMontage && FirstPersonAnimInstance)
		{
			if (FirstPersonAnimInstance->Montage_Play(*ServerFirstPersonAttackMontage))
			{
				FirstPersonAnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &ASCharacterBase::OnMontageNotifyBeginTryApplyDamage);
				FirstPersonAnimInstance->OnPlayMontageNotifyEnd.AddDynamic(this, &ASCharacterBase::OnMontageNotifyEndSetWeaponIdleState);
			}
		}

		// Some repeated code, trying to avoid doing TMap.Find() when montage will not be used
		if (!IsLocallyControlled() && ThirdPersonAnimInstance)
		{
			auto ThirdPersonAttackMontage = MeleeWeaponData[CurrentWeaponIndex].ThirdPeronAttackMontages.Find(MeleeAttack);
			if (*ThirdPersonAttackMontage)
			{
				ThirdPersonAnimInstance->Montage_Play(*ThirdPersonAttackMontage);
			}
		}
	}
	else
	{
		if (IsLocallyControlled() && FirstPersonAnimInstance)	// Autonomous Proxy
		{
			auto FirstPersonAttackMontage = MeleeWeaponData[CurrentWeaponIndex].FirstPeronAttackMontages.Find(MeleeAttack);
			if (*FirstPersonAttackMontage)
			{
				FirstPersonAnimInstance->Montage_Play(*FirstPersonAttackMontage);
			}
		}
		else if (ThirdPersonAnimInstance)	// Simulated Proxy
		{
			auto ThirdPersonAttackMontage = MeleeWeaponData[CurrentWeaponIndex].ThirdPeronAttackMontages.Find(MeleeAttack);
			if (*ThirdPersonAttackMontage)
			{
				ThirdPersonAnimInstance->Montage_Play(*ThirdPersonAttackMontage);
			}
		}
	}
}


void ASCharacterBase::OnMontageNotifyBeginTryApplyDamage(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (FirstPersonAnimInstance)
	{
		FirstPersonAnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &ASCharacterBase::OnMontageNotifyBeginTryApplyDamage);
	}

	if (GetLocalRole() == ENetRole::ROLE_Authority && MeleeWeaponData[CurrentWeaponIndex].FPPMeleeWeapon)
	{
		MeleeWeaponData[CurrentWeaponIndex].FPPMeleeWeapon->SetCanCauseDamage(true);
	}
}


void ASCharacterBase::OnMontageNotifyEndSetWeaponIdleState(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (FirstPersonAnimInstance)
	{
		FirstPersonAnimInstance->OnPlayMontageNotifyEnd.RemoveDynamic(this, &ASCharacterBase::OnMontageNotifyEndSetWeaponIdleState);
	}

	if (GetLocalRole() == ENetRole::ROLE_Authority && MeleeWeaponData[CurrentWeaponIndex].FPPMeleeWeapon)
	{
		MeleeWeaponData[CurrentWeaponIndex].FPPMeleeWeapon->SetCanCauseDamage(false);
	}
}


/*************************************************************************/
/* Melee Weapon Block */
/*************************************************************************/
void ASCharacterBase::WeaponBlockStart()
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


void ASCharacterBase::WeaponBlockStop()
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


/*************************************************************************/
/* Weapon magic charge change */
/*************************************************************************/
void ASCharacterBase::WeaponMagicChargeChange(bool Value)
{
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
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


void ASCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASCharacterBase, bIsBlocking);
	DOREPLIFETIME(ASCharacterBase, bCurrentWeaponIsMagicCharged);
}

