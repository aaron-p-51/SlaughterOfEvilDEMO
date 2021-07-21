// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SEnemyBase.h"

void ASEnemyBase::MeleeAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("EnemyBase.cpp MeleeAttack"));
	if (AttackForwardMontage && GetMesh())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(AttackForwardMontage);
	}
}
