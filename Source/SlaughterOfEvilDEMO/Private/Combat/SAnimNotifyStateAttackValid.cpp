// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/SAnimNotifyStateAttackValid.h"

// Game Includes
#include "Player/SCharacterBase.h"


void USAnimNotifyStateAttackValid::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	if (!MeshComp) return;

	ASCharacterBase* MeshOwner = Cast<ASCharacterBase>(MeshComp->GetOwner());
	if (MeshOwner)
	{
		MeshOwner->TrySetMeleeAttackState(EMeleeAttackState::EMAS_CauseDamage);
	}
}

void USAnimNotifyStateAttackValid::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	if (!MeshComp) return;

	ASCharacterBase* MeshOwner = Cast<ASCharacterBase>(MeshComp->GetOwner());
	if (MeshOwner)
	{
		MeshOwner->TrySetMeleeAttackState(EMeleeAttackState::EMAS_Attacking);
	}
}
