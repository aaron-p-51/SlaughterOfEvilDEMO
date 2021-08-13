// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/SAnimNotifyMeleeChargeReady.h"

// Game Includes
#include "Player/SCharacterBase.h"

void USAnimNotifyMeleeChargeReady::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (!MeshComp) return;

	ASCharacterBase* MeshOwner = Cast<ASCharacterBase>(MeshComp->GetOwner());
	if (MeshOwner)
	{
		ASMeleeWeapon* Weapon = MeshOwner->GetCurrentMeleeWeapon();
		if (Weapon)
		{
			if (IsMagicCharge)
			{
				Weapon->OnMagicChargeReady();
			}
		}
	}
}
