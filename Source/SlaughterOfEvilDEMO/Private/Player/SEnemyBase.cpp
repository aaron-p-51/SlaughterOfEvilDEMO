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

	/*if (AIGroupControlComp)
	{
		EGroupField Field = bIsNearField ? EGroupField::EGF_Near : EGroupField::EGF_Far;
		AIGroupControlComp->GroupControllerData.AssignedGroupField = Field;
	}*/
}


void ASEnemyBase::MeleeAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("EnemyBase.cpp MeleeAttack"));
	if (AttackForwardMontage && GetMesh())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(AttackForwardMontage);
	}
}
