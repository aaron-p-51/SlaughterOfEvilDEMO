// Copyright Epic Games, Inc. All Rights Reserved.

#include "SlaughterOfEvilDEMOGameMode.h"
#include "SlaughterOfEvilDEMOHUD.h"
#include "SlaughterOfEvilDEMOCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASlaughterOfEvilDEMOGameMode::ASlaughterOfEvilDEMOGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ASlaughterOfEvilDEMOHUD::StaticClass();
}
