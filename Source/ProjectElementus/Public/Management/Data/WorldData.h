// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "WorldData.generated.h"

/**
 *
 */
USTRUCT(BlueprintType, Category = "Custom GAS | Data")
struct FFeatureLoadingInfo
{
	GENERATED_BODY()

	/* If enabled, will load the game feature when the game starts */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bActivate;

	/* Associated Game Feature to manage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName GameFeatureName;
};

USTRUCT(BlueprintType, Category = "Custom GAS | Data")
struct FWorldData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	/* Modular GameFeatures to load when the game initializes and all the actors was spawned - This will be used in multiplayer, which depends to wait player connection */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FFeatureLoadingInfo DefaultGameFeaturesToInitialize;
};
