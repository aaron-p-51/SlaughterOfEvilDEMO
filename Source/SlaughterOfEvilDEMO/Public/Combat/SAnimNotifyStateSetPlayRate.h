// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "SAnimNotifyStateSetPlayRate.generated.h"

/**
 * 
 */
UCLASS()
class SLAUGHTEROFEVILDEMO_API USAnimNotifyStateSetPlayRate : public UAnimNotifyState
{
	GENERATED_BODY()

private:

	float CurrentCurveY = 0.f;

	float Steps;

	float StateLength;

public:

	UPROPERTY(EditAnywhere)
	float Speed;

	UPROPERTY(EditAnywhere)
	bool bUseCurve;

	UPROPERTY(EditAnywhere)
	UCurveFloat* SpeedCurve;



private:

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
