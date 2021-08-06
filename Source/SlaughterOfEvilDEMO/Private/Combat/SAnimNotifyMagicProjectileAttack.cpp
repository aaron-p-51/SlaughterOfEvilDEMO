// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/SAnimNotifyMagicProjectileAttack.h"


#include "SProjectileMagicCaster.h"

void USAnimNotifyMagicProjectileAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (!MeshComp) return;

	auto MeshOwner = Cast<ISProjectileMagicCaster>(MeshComp->GetOwner());
	if (MeshOwner)
	{
		MeshOwner->SpawnMagicAttackProjectile();
	}
}
