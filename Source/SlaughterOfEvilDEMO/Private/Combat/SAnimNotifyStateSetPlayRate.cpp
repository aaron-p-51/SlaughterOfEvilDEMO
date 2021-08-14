// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/SAnimNotifyStateSetPlayRate.h"

void USAnimNotifyStateSetPlayRate::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	auto AnimInst = MeshComp->GetAnimInstance();
	if (AnimInst && SpeedCurve)
	{
		AnimInst->GetCurrentActiveMontage();
		AnimInst->Montage_SetPlayRate(AnimInst->GetCurrentActiveMontage(), SpeedCurve->GetFloatValue(0.f));
	}

	StateLength = TotalDuration;
	Steps = 0.f;
	
}


void USAnimNotifyStateSetPlayRate::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);
	Steps += FrameDeltaTime;
	TRange<float> CurveRange;
	TRange<float> StateRange;
	CurveRange.SetLowerBound(0.f);
	CurveRange.SetUpperBound(1.f);
	StateRange.SetLowerBound(0.f);
	StateRange.SetUpperBound(StateLength);

	float newLoowkup = 0.f + ((1.f- 0.f) / (StateLength - 0.f)) * Steps;		// = FMath::GetMappedRangeValueClamped(StateRange, CurveRange, Steps);
	
	auto AnimInst = MeshComp->GetAnimInstance();
	if (AnimInst && SpeedCurve)
	{
		AnimInst->GetCurrentActiveMontage();
		AnimInst->Montage_SetPlayRate(AnimInst->GetCurrentActiveMontage(), SpeedCurve->GetFloatValue(newLoowkup));
	
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
