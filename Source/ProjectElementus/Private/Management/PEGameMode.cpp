// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Management/PEGameMode.h"
#include "Actors/Character/PEHUD.h"
#include "Actors/Character/PEPlayerState.h"
#include "Actors/Character/PEPlayerController.h"

APEGameMode::APEGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), bCanRespawn(true)
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBP_ClassRef(TEXT("/Game/Main/Blueprints/Actors/Pawns/BP_Character"));
	if constexpr (&PlayerPawnBP_ClassRef.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBP_ClassRef.Class;
	}

	PlayerStateClass = APEPlayerState::StaticClass();
	PlayerControllerClass = APEPlayerController::StaticClass();
	HUDClass = APEHUD::StaticClass();
}
