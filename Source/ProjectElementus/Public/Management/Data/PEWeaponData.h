#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PEWeaponData.generated.h"

/**
 *
 */
UCLASS(NotBlueprintable, NotPlaceable, Category = "Custom Data | Data Assets")
class PROJECTELEMENTUS_API UPEWeaponData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPEWeaponData(const FObjectInitializer& ObjectInitializer);

	FORCEINLINE virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("Weapon Data", GetFName());
	}
};
