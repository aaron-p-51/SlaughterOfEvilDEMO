// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SlaughterOfEvilDEMOHUD.generated.h"

UCLASS()
class ASlaughterOfEvilDEMOHUD : public AHUD
{
	GENERATED_BODY()

public:
	ASlaughterOfEvilDEMOHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

