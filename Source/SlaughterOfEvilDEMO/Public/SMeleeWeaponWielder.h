// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/SMeleeWeapon.h"
#include "UObject/Interface.h"
#include "SMeleeWeaponWielder.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USMeleeWeaponWielder : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SLAUGHTEROFEVILDEMO_API ISMeleeWeaponWielder
{

	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:


	virtual ASMeleeWeapon* GetCurrentMeleeWeapon() = 0;

	virtual bool IsBlocking() = 0;
	virtual void WeaponMagicChargeChange(bool Value) = 0;

	
	virtual void MeleeAttack() = 0;
	virtual void MeleeAttackCanCauseDamage(bool Value) = 0;
	virtual void MeleeAttackFinished() = 0;

	virtual void MeleeStartBlock() = 0;
	virtual void MeleeStopBlock() = 0;
	//virtual bool TrySetMagicCharge(bool Charged) = 0;


};
