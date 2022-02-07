#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PEEnemyData.generated.h"

/**
 *
 */
UCLASS(NotBlueprintable, NotPlaceable, Category = "Custom Data | Data Assets")
class PROJECTELEMENTUS_API UPEEnemyData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPEEnemyData(const FObjectInitializer& ObjectInitializer);

	FORCEINLINE virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("Enemy Data", GetFName());
	}
};
