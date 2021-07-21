// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/SAnimNotifyStateSetPlayRate.h"

void USAnimNotifyStateSetPlayRate::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	auto AnimInst = MeshComp->GetAnimInstance();
	if (AnimInst)
	{
		AnimInst->GetCurrentActiveMontage();
		AnimInst->Montage_SetPlayRate(AnimInst->GetCurrentActiveMontage(), Speed);
	}

	
}

void USAnimNotifyStateSetPlayRate::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	auto AnimInst = MeshComp->GetAnimInstance();
	if (AnimInst)
	{
		AnimInst->GetCurrentActiveMontage();
		AnimInst->Montage_SetPlayRate(AnimInst->GetCurrentActiveMontage(), 1.f);
	}
}
