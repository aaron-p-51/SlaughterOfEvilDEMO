// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/SAnimNotifyStateWeaponCosmetics.h"

// Game Includes
#include "Player/SCharacterBase.h"

void USAnimNotifyStateWeaponCosmetics::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	if (!MeshComp) return;

	ASCharacterBase* MeshOwner = Cast<ASCharacterBase>(MeshComp->GetOwner());
	if (MeshOwner)
	{
		ASMeleeWeapon* Weapon = MeshOwner->GetCurrentMeleeWeapon();
		if (Weapon)
		{
			Weapon->EnableCosmeticWeaponTrace(true);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("NoWeapon"));
		}
	}
}


void USAnimNotifyStateWeaponCosmetics::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	if (!MeshComp) return;

	ASCharacterBase* MeshOwner = Cast<ASCharacterBase>(MeshComp->GetOwner());
	if (MeshOwner)
	{
		ASMeleeWeapon* Weapon = MeshOwner->GetCurrentMeleeWeapon();
		if (Weapon)
		{
			Weapon->EnableCosmeticWeaponTrace(false);
		}
	}
}
