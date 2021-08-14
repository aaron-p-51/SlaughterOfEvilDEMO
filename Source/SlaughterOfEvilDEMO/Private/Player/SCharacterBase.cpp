 //Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SCharacterBase.h"

// Engine Includes
#include "AIController.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"



// Game Includes
#include "Weapons/SWeaponBase.h"
#include "Weapons/SMeleeWeapon.h"
#include "Components/SHealthComponent.h"

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

	HealthComponent = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));


	
	bReplicates = true;
	SetReplicateMovement(true);

}


void ASCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
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

	if (HealthComponent)
	{
		HealthComponent->OnHealthChange.AddDynamic(this, &ASCharacterBase::HealthCompOnHealthChange);
	}
}


UAnimInstance* ASCharacterBase::GetLocalAnimInstance()
{
	return (IsFirstPerson()) ? FirstPersonMesh->GetAnimInstance() : GetMesh()->GetAnimInstance();
}


USkeletalMeshComponent* ASCharacterBase::GetLocalMesh()
{
	return (IsFirstPerson()) ? FirstPersonMesh : GetMesh();
}


void ASCharacterBase::OnRep_CurrentWeapon(ASWeaponBase* LastWeapon)
{
	SetCurrentWeapon(CurrentWeapon, LastWeapon);
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


ASMeleeWeapon* ASCharacterBase::GetCurrentMeleeWeapon()
{
	if (CurrentWeapon)
	{
		return Cast<ASMeleeWeapon>(CurrentWeapon);
	}

	return nullptr;
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

	PlayerInputComponent->BindAction(TEXT("Block"), EInputEvent::IE_Pressed, this, &ASCharacterBase::MeleeStartBlock);
	PlayerInputComponent->BindAction(TEXT("Block"), EInputEvent::IE_Released, this, &ASCharacterBase::MeleeStopBlock);

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
	if (CurrentWeapon)
	{
		CurrentWeapon->Use();
	}
}


FTransform ASCharacterBase::GetMagicLaunchTransform()
{
	if (Camera)
	{
		return Camera->GetComponentTransform();
	}
	else
	{
		return GetActorTransform();
	}
}





/*************************************************************************/
/* Melee Weapon Block */
/*************************************************************************/

void ASCharacterBase::MeleeStartBlock()
{
	if (CurrentWeapon)
	{
		auto MeleeWeapon = Cast<ASMeleeWeapon>(CurrentWeapon);
		if (MeleeWeapon)
		{
			MeleeWeapon->StartBlock();
		}
	}
}


void ASCharacterBase::MeleeStopBlock()
{
	if (CurrentWeapon)
	{
		auto MeleeWeapon = Cast<ASMeleeWeapon>(CurrentWeapon);
		if (MeleeWeapon)
		{
			MeleeWeapon->StopBlock();
		}
	}
}


bool ASCharacterBase::IsBlocking()
{
	if (CurrentWeapon)
	{
		auto MeleeWeapon = Cast<ASMeleeWeapon>(CurrentWeapon);
		if (MeleeWeapon && MeleeWeapon->GetIsBlocking())
		{
			return true;
		}
	}

	return false;
}



/*************************************************************************/
/* Weapon magic  */
/*************************************************************************/

void ASCharacterBase::StartUseWeaponMagic()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartUseMagicCharge();
	}
}


void ASCharacterBase::FinishUseWeaponMagic()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->FinishUseMagicCharge();
	}

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


void ASCharacterBase::HealthCompOnHealthChange(USHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* HealthChangeCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("Damage detected"));
	UE_LOG(LogTemp, Warning, TEXT("%s has been damaged by %s, health is now at %d"), *GetName(), *HealthChangeCauser->GetName(), Health);
}

TWeakObjectPtr<USceneComponent> ASCharacterBase::GetProjectileHomingTarget()
{
	return TWeakObjectPtr<USceneComponent>(ProjectileHomingTarget);
}


bool ASCharacterBase::IsAIControlled() const
{
	return (Cast<APlayerController>(GetController())) ? false : true;
}


void ASCharacterBase::SetProjectileHomingTargetLocation(bool IsCrouching)
{
	if (!ProjectileHomingTarget) return;

	float HeightOffset = (IsCrouching) ? CrouchedEyeHeight : BaseEyeHeight;
	ProjectileHomingTarget->AddLocalOffset(FVector(0.f, 0.f, HeightOffset));
}


float ASCharacterBase::PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate /*= 1.f*/, FName StartSectionName /*= NAME_None*/)
{
	auto LocalMesh = GetLocalMesh();
	float Duration = 0.f;
	if (LocalMesh && LocalMesh->AnimScriptInstance)
	{
		Duration = LocalMesh->AnimScriptInstance->Montage_Play(AnimMontage);
	}

	UE_LOG(LogTemp, Warning, TEXT("ASCharacterBase::PlayAnimMontage duration: %f"), Duration);

	return Duration;
}


float ASCharacterBase::PlayFirstPersonMontageOnServer(class UAnimMontage* AnimMontage)
{
	float Duration = 0.f;
	if (GetLocalRole() == ENetRole::ROLE_Authority && FirstPersonMesh && FirstPersonMesh->AnimScriptInstance)
	{
		Duration = FirstPersonMesh->AnimScriptInstance->Montage_Play(AnimMontage);
	}

	return Duration;
}


bool ASCharacterBase::IsMeleeWeaponMagicChargeReady()
{
	auto CurrentMeleeWeapon = GetCurrentMeleeWeapon();
	return CurrentMeleeWeapon && CurrentMeleeWeapon->GetIsWeaponMagicChargeReady();
}


void ASCharacterBase::SpawnDefaultInventory()
{
	// Weapons are set to replicate so only spawn on server
	if (GetLocalRole() < ENetRole::ROLE_Authority)
	{
		return;
	}

	WeaponInventory.Empty();
	int32 MeleeWeaponClasses = WeaponInventoryClasses.Num();
	for (int32 i = 0; i < MeleeWeaponClasses; ++i)
	{
		if (WeaponInventoryClasses[i])
		{
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			ASWeaponBase* NewWeapon = GetWorld()->SpawnActor<ASWeaponBase>(WeaponInventoryClasses[i], SpawnParameters);
			AddWeapon(NewWeapon);
		}
	}

	if (WeaponInventory.Num() > 0)
	{
		EquipWeapon(WeaponInventory[0]);
	}
}


void ASCharacterBase::AddWeapon(ASWeaponBase* Weapon)
{
	if (Weapon && GetLocalRole() == ENetRole::ROLE_Authority)
	{
		Weapon->OnEnterInventory(this);
		WeaponInventory.AddUnique(Weapon);
	}
}


void ASCharacterBase::EquipWeapon(ASWeaponBase* Weapon)
{
	if (Weapon)
	{
		if (GetLocalRole() < ENetRole::ROLE_Authority)
		{
			ServerEquipWeapon(Weapon);
		}
		else
		{
			SetCurrentWeapon(Weapon, CurrentWeapon);
		}
	}
}


void ASCharacterBase::SetCurrentWeapon(ASWeaponBase* NewWeapon, ASWeaponBase* PreviousWeapon)
{
	ASWeaponBase* LocalPreviousWeapon = nullptr;

	if (PreviousWeapon != nullptr)
	{
		LocalPreviousWeapon = PreviousWeapon;
	}
	else if (NewWeapon != PreviousWeapon)
	{
		LocalPreviousWeapon = CurrentWeapon;
	}

	// TODO unequip previous weapon


	CurrentWeapon = NewWeapon;

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
	for (int32 i = WeaponInventory.Num() - 1; i >= 0; i--)
	{
		ASWeaponBase* Weapon = WeaponInventory[i];
		if (Weapon)
		{
			//RemoveWeapon(Weapon);
			WeaponInventory.RemoveSingle(Weapon);
		
		}
	}
}


void ASCharacterBase::ServerEquipWeapon_Implementation(ASWeaponBase* Weapon)
{
	EquipWeapon(Weapon);
}


void ASCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	DOREPLIFETIME(ASCharacterBase, CurrentWeapon);

	// only to local owner: weapon change requests are locally instigated, other clients don't need it
	DOREPLIFETIME_CONDITION(ASCharacterBase, WeaponInventory, COND_OwnerOnly);
	

}



