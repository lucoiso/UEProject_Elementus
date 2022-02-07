#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "PEConsumableData.generated.h"

class UGameplayEffect;
class UNiagaraSystem;
/**
 *
 */
UCLASS(NotBlueprintable, NotPlaceable, Category = "Custom Data | Data Assets")
class PROJECTELEMENTUS_API UPEConsumableData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPEConsumableData(const FObjectInitializer& ObjectInitializer);

	FORCEINLINE virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("Consumable Data", GetFName());
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom Properties | Defaults")
	TSoftObjectPtr<UStaticMesh> ObjectMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom Properties | Defaults")
	TSoftObjectPtr<UNiagaraSystem> ObjectVFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom Properties | Defaults")
	TSoftClassPtr<UGameplayEffect> ObjectEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom Properties | Defaults")
	FGameplayTagContainer RequirementsTags;
};
