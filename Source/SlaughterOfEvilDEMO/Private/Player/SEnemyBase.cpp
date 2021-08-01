// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SEnemyBase.h"

#include "AI/AIGroupControlComponent.h"



ASEnemyBase::ASEnemyBase()
{
	AIGroupControlComp = CreateDefaultSubobject<UAIGroupControlComponent>(TEXT("AIGroupControlComp"));
}


void ASEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	if (AIGroupControlComp)
	{
		AIGroupControlComp->OnTriggerAttack.AddDynamic(this, &ASEnemyBase::AIGroupControllerTriggerMeleeAttack);
		AIGroupControlComp->OnTriggerAttack.AddDynamic(this, &ASEnemyBase::AIGroupControllerTriggerRangeAttack);
	}

}


void ASEnemyBase::AIGroupControllerTriggerMeleeAttack(AActor* GroupTarget)
{
	UE_LOG(LogTemp, Warning, TEXT("AIGroupControllerTriggerMeleeAttack"));
	MeleeAttack();
}

void ASEnemyBase::AIGroupControllerTriggerRangeAttack(AActor* GroupTarget)
{
	UE_LOG(LogTemp, Warning, TEXT("AIGroupControllerTriggerRangeAttack"));
}

void ASEnemyBase::MeleeAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("EnemyBase.cpp MeleeAttack"));
	if (AttackForwardMontage && GetMesh())
	{
		auto AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			if (AnimInstance->IsAnyMontagePlaying() == false)
			{
				AnimInstance->Montage_Play(AttackForwardMontage);
			}
		}
	
	}
}
