// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include <CoreMinimal.h>
#include <GameFramework/GameModeBase.h>
#include "PEGameMode.generated.h"

/**
 *
 */
UCLASS(MinimalAPI, Category = "Project Elementus | Classes")
class APEGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	explicit APEGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/* Define if the players can respawn or not in the current context */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Project Elementus | Properties")
	bool bCanRespawn;
};
