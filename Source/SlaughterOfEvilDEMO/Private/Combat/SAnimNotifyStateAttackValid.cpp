// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/SAnimNotifyStateAttackValid.h"

// Game Includes
#include "SMeleeWeaponWielder.h"


void USAnimNotifyStateAttackValid::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	if (!MeshComp) return;

	auto MeshOwner = Cast<ISMeleeWeaponWielder>(MeshComp->GetOwner());
	if (MeshOwner)
	{
		MeshOwner->MeleeAttackCanCauseDamage(true);
	}
}

void USAnimNotifyStateAttackValid::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	if (!MeshComp) return;

	auto MeshOwner = Cast<ISMeleeWeaponWielder>(MeshComp->GetOwner());
	if (MeshOwner)
	{
		MeshOwner->MeleeAttackCanCauseDamage(false);
	}
}
