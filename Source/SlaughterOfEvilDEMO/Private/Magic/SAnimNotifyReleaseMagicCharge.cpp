// Fill out your copyright notice in the Description page of Project Settings.


#include "Magic/SAnimNotifyReleaseMagicCharge.h"

// Game includes
#include "Player/SCharacterBase.h"

void USAnimNotifyReleaseMagicCharge::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (!MeshComp) return;

	ASCharacterBase* MeshOwner = Cast<ASCharacterBase>(MeshComp->GetOwner());
	if (MeshOwner)
	{
		ASMeleeWeapon* Weapon = MeshOwner->GetCurrentMeleeWeapon();
		if (Weapon)
		{
			Weapon->OnMagicChargeRelease();
		}
	}
}
