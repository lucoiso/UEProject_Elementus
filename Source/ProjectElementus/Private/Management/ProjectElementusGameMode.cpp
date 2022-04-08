// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Management/ProjectElementusGameMode.h"
#include "Management/PEHUD.h"

#include "Actors/Character/PEPlayerState.h"
#include "Actors/Character/PEPlayerController.h"

AProjectElementusGameMode::AProjectElementusGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBP_ClassRef(
		TEXT("/Game/Main/Blueprints/Actors/Pawns/BP_Character_01"));
#if __cplusplus > 201402L // Check if C++ > C++14
	if constexpr (&PlayerPawnBP_ClassRef.Class != nullptr)
#else
	if (&PlayerPawnBP_ClassRef.Class != nullptr)
#endif
	{
		DefaultPawnClass = PlayerPawnBP_ClassRef.Class;
	}

	PlayerStateClass = APEPlayerState::StaticClass();
	PlayerControllerClass = APEPlayerController::StaticClass();
	HUDClass = APEHUD::StaticClass();
}