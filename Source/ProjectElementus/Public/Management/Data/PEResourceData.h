#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PEResourceData.generated.h"

/**
 *
 */
UCLASS(NotBlueprintable, NotPlaceable, Category = "Custom Data | Data Assets")
class PROJECTELEMENTUS_API UPEResourceData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPEResourceData(const FObjectInitializer& ObjectInitializer);

	FORCEINLINE virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("Resource Data", GetFName());
	}
};
