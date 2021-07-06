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
	CurrentWeapon = NO_WEAPON_SET;
	
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
	for (int32 i = 0; i < WeaponData.Num(); ++i)
	{
		FTransform WeaponSpawnTransform(GetActorTransform());
		WeaponData[i].MeleeWeapon = Cast<ASMeleeWeaponBase>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, WeaponData[i].MeleeWeaponClass, WeaponSpawnTransform));
		if (WeaponData[i].MeleeWeapon)
		{
			if (WeaponData[i].bIsStartingWeapon && CurrentWeapon == NO_WEAPON_SET)
			{
				CurrentWeapon = i;
			}
			else if (WeaponData[i].bIsStartingWeapon && CurrentWeapon > NO_WEAPON_SET)
			{
				UE_LOG(LogTemp, Warning, TEXT("Multiple starting weapons set for %s. Starting weapon will be set to %s"), *GetName(), *WeaponData[i].MeleeWeaponClass->GetName());
			}


			WeaponData[i].MeleeWeapon->SetOwner(this);
			WeaponData[i].MeleeWeapon->SetInstigator(this);
			if (IsLocallyControlled() && FirstPersonMesh)
			{
				WeaponData[i].MeleeWeapon->AttachToComponent(FirstPersonMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponData[i].MeleeWeaponSocketName);
			}
			else if (GetMesh())
			{
				WeaponData[i].MeleeWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponData[i].MeleeWeaponSocketName);
			}

			UGameplayStatics::FinishSpawningActor(WeaponData[i].MeleeWeapon, WeaponSpawnTransform);
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
	// Client side check see if weapon state is already attacking
	if (WeaponData[CurrentWeapon].MeleeWeapon->GetMeleeWeaponState() != EMeleeWeaponState::EMWS_Attacking)
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
	if (WeaponData[CurrentWeapon].MeleeWeapon->TrySetMeleeWeaponState(EMeleeWeaponState::EMWS_Attacking))
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
	
	FApplyDamageData ApplyDamageData;
	ApplyDamageData.BaseDamage = 1.f;
	ApplyDamageData.DamageSphereRadius = 80.f;
	

	WeaponData[CurrentWeapon].MeleeWeapon->SetApplyDamageData(ApplyDamageData);

	FVector DamageSphereLocation = GetOwner()->GetActorLocation();
	DamageSphereLocation.Z += WeaponData[CurrentWeapon].WeaponUseVerticalOffset;
	DamageSphereLocation +=	GetActorForwardVector() * WeaponData[CurrentWeapon].WeaponUseForwardOffset;
	WeaponData[CurrentWeapon].MeleeWeapon->TryApplyDamage(DamageSphereLocation);

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

	if (GetLocalRole() == ENetRole::ROLE_Authority && WeaponData[CurrentWeapon].MeleeWeapon)
	{
		WeaponData[CurrentWeapon].MeleeWeapon->TrySetMeleeWeaponState(EMeleeWeaponState::EMWS_Idle);
	}
}



