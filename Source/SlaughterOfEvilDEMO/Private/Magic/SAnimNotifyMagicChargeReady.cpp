// Fill out your copyright notice in the Description page of Project Settings.


#include "Magic/SAnimNotifyMagicChargeReady.h"

// Game includes
#include "Player/SCharacterBase.h"

void USAnimNotifyMagicChargeReady::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	
	if (!MeshComp) return;

	if (MeshComp->GetOwner())
	{
		auto CharacterBase = Cast<ASCharacterBase>(MeshComp->GetOwner());
		if (CharacterBase)
		{
			CharacterBase->TrySetMagicUseState(EMagicUseState::EMUS_Ready);
		}
	}
}
