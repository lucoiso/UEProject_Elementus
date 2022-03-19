// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Management/ProjectElementusGameMode.h"
#include "Actors/Character/PEPlayerState.h"
#include "Actors/Character/PEPlayerController.h"

AProjectElementusGameMode::AProjectElementusGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(
		TEXT("/Game/Main/Blueprints/Actors/Pawns/BP_Character_01"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	PlayerStateClass = APEPlayerState::StaticClass();
	PlayerControllerClass = APEPlayerController::StaticClass();
}