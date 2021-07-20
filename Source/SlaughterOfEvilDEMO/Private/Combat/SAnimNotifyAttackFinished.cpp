// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/SAnimNotifyAttackFinished.h"

// Game Includes
#include "Player/SCharacterBase.h"

void USAnimNotifyAttackFinished::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (!MeshComp) return;

	auto MeshOwner = Cast<ASCharacterBase>(MeshComp->GetOwner());
	if (MeshOwner)
	{
		MeshOwner->TrySetMeleeAttackState(EMeleeAttackState::EMAS_Idle);	
	}

}
