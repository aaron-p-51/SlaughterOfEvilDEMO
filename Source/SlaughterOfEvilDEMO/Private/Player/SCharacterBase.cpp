// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SCharacterBase.h"

// Engine Includes
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"


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

	
	ThirdPersonAnimInstance = GetMesh()->GetAnimInstance();
	CurrentWeaponIndex = NO_WEAPON_SET;
	
	SetReplicates(true);
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
}


void ASCharacterBase::SpawnStartingWeapons()
{
	for (int32 i = 0; i < MeleeWeaponData.Num(); ++i)
	{
		FTransform WeaponSpawnTransform(GetActorTransform());
		MeleeWeaponData[i].MeleeWeapon = Cast<ASMeleeWeaponBase>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, MeleeWeaponData[i].MeleeWeaponClass, WeaponSpawnTransform));
		if (MeleeWeaponData[i].MeleeWeapon)
		{
			MeleeWeaponData[i].MeleeWeapon->SetOwner(this);
			MeleeWeaponData[i].MeleeWeapon->SetInstigator(this);
			if (IsLocallyControlled() && FirstPersonMesh)
			{
				MeleeWeaponData[i].MeleeWeapon->AttachToComponent(FirstPersonMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, MeleeWeaponData[i].MeleeWeaponSocketName);
			}
			else if (GetMesh())
			{
				MeleeWeaponData[i].MeleeWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, MeleeWeaponData[i].MeleeWeaponSocketName);
			}

			UGameplayStatics::FinishSpawningActor(MeleeWeaponData[i].MeleeWeapon, WeaponSpawnTransform);
		}
	}

	for (int32 i = 0; i < MeleeWeaponData.Num(); ++i)
	{
		if (MeleeWeaponData[i].bIsStartingWeapon && CurrentWeaponIndex == NO_WEAPON_SET)
		{
			CurrentWeaponIndex = i;
			CurrentWeapon = MeleeWeaponData[i].MeleeWeapon;
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

	PlayerInputComponent->BindAction(TEXT("Action"), EInputEvent::IE_Pressed, this, &ASCharacterBase::WeaponAction);
	
}


void ASCharacterBase::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector() * Value);	
}


void ASCharacterBase::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector() * Value);
}


void ASCharacterBase::WeaponAction()
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
		if (FirstPersonAnimInstance)
		{
			auto AttackMontage = FirstPersonLongswordMontages.Find(MeleeAttack);

			if (AttackMontage && FirstPersonAnimInstance->Montage_Play(*AttackMontage) != MONTAGE_PLAY_FAIL)
			{
				FirstPersonAnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &ASCharacterBase::OnMontageNotifyBeginTryApplyDamage);
				FirstPersonAnimInstance->OnPlayMontageNotifyEnd.AddDynamic(this, &ASCharacterBase::OnMontageNotifyEndSetWeaponIdleState);
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

	if (GetLocalRole() == ENetRole::ROLE_Authority && CurrentWeapon)
	{
		CurrentWeapon->SetCanCauseDamage(true);
	}
}


void ASCharacterBase::OnMontageNotifyEndSetWeaponIdleState(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (FirstPersonAnimInstance)
	{
		FirstPersonAnimInstance->OnPlayMontageNotifyEnd.RemoveDynamic(this, &ASCharacterBase::OnMontageNotifyEndSetWeaponIdleState);
	}

	if (GetLocalRole() == ENetRole::ROLE_Authority && CurrentWeapon)
	{
		CurrentWeapon->SetCanCauseDamage(false);
	}
}



