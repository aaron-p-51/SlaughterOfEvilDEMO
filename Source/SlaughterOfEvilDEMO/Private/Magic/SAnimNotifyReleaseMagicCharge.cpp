// Fill out your copyright notice in the Description page of Project Settings.


#include "Magic/SAnimNotifyReleaseMagicCharge.h"

// Game includes
#include "Player/SCharacterBase.h"

void USAnimNotifyReleaseMagicCharge::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;

	if (MeshComp->GetOwner())
	{
		auto CharacterBase = Cast<ASCharacterBase>(MeshComp->GetOwner());
		if (CharacterBase)
		{
			CharacterBase->TrySetMagicUseState(EMagicUseState::EMUS_Finish);
		}
	}
}
