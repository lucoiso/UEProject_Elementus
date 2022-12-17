// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include <CoreMinimal.h>
#include <GameplayTagContainer.h>
#include <Engine/DataAsset.h>
#include "GAS/System/PEEffectData.h"
#include "PEConsumableData.generated.h"

class UNiagaraSystem;
/**
 *
 */
UCLASS(NotBlueprintable, NotPlaceable, Category = "Project Elementus | Classes")
class PROJECTELEMENTUS_API UPEConsumableData final : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	explicit UPEConsumableData(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	FORCEINLINE virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(TEXT("PE_ConsumableData"), *("Consumable_" + FString::FromInt(ConsumableId)));
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus", meta = (AssetBundles = "Data"))
	int32 ConsumableId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus", meta = (AssetBundles = "SoftData"))
	TSoftObjectPtr<UStaticMesh> ObjectMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus", meta = (AssetBundles = "SoftData"))
	TSoftObjectPtr<UNiagaraSystem> ObjectVFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus", meta = (AssetBundles = "Gameplay", TitleProperty = "{EffectClass}"))
	TArray<FGameplayEffectGroupedData> ConsumableEffects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus", meta = (AssetBundles = "Gameplay"))
	FGameplayTagContainer RequirementsTags;
};
