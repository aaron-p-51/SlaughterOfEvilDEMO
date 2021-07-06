// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SCharacterBase.h"

// Engine Includes
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"


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

	MeleeWeaponSocketName = TEXT("RightHandMeleeWeaponSocket");
	
	ThirdPersonAnimInstance = GetMesh()->GetAnimInstance();
	
	SetReplicates(true);
	SetReplicateMovement(true);

}

// Called when the game starts or when spawned
void ASCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	MeleeWeapon = GetWorld()->SpawnActor<ASMeleeWeaponBase>(MeleeWeaponClass);
	if (MeleeWeapon)
	{
		if (IsLocallyControlled() && FirstPersonMesh)
		{
			MeleeWeapon->AttachToComponent(FirstPersonMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, MeleeWeaponSocketName);
		}
		else if (GetMesh())
		{
			GetMesh()->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, MeleeWeaponSocketName);
		}
	}
	
	// Cache the Animation Instance for first person mesh. DO NOT place this in
	// constructor, will not be initialized properly
	if (FirstPersonMesh)
	{
		FirstPersonAnimInstance = FirstPersonMesh->GetAnimInstance();
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
	// Client side check see if weapon state is already attacking
	if (MeleeWeapon->GetMeleeWeaponState() != EMeleeWeaponState::EMWS_Attacking)
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
}


void ASCharacterBase::ServerTryMeleeAttack_Implementation()
{
	if (MeleeWeapon->TrySetMeleeWeaponState(EMeleeWeaponState::EMWS_Attacking))
	{
		MeleeAttackDirection = GetMeleeAttackDirection();
		MulticastPlayMeleeAttackMontage(MeleeAttackDirection);
	}
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
	UE_LOG(LogTemp, Warning, TEXT("Notify Hit"));
	
	if (FirstPersonAnimInstance)
	{
		FirstPersonAnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &ASCharacterBase::OnMontageNotifyBeginTryApplyDamage);
	}
}


void ASCharacterBase::OnMontageNotifyEndSetWeaponIdleState(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (FirstPersonAnimInstance)
	{
		FirstPersonAnimInstance->OnPlayMontageNotifyEnd.RemoveDynamic(this, &ASCharacterBase::OnMontageNotifyEndSetWeaponIdleState);
	}

	if (GetLocalRole() == ENetRole::ROLE_Authority && MeleeWeapon)
	{
		MeleeWeapon->TrySetMeleeWeaponState(EMeleeWeaponState::EMWS_Idle);
	}
}

