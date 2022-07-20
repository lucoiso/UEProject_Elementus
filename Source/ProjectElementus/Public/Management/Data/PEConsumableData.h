// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GAS/System/PEAbilityData.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "PEConsumableData.generated.h"

class UGameplayEffect;
class UNiagaraSystem;
/**
 *
 */
UCLASS(NotBlueprintable, NotPlaceable, Category = "Project Elementus | Classes")
class PROJECTELEMENTUS_API UPEConsumableData final : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	explicit UPEConsumableData(const FObjectInitializer& ObjectInitializer);

	FORCEINLINE virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(TEXT("PE_ConsumableData"),
		                       *("Consumable_" + FString::FromInt(ConsumableId)));
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties",
		meta = (AssetBundles = "Data"))
	int32 ConsumableId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties",
		meta = (AssetBundles = "Actor"))
	TSoftObjectPtr<UStaticMesh> ObjectMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties",
		meta = (AssetBundles = "Actor"))
	TSoftObjectPtr<UNiagaraSystem> ObjectVFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties",
		meta = (AssetBundles = "Effects"))
	TArray<FGameplayEffectGroupedData> ConsumableEffects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties",
		meta = (AssetBundles = "Tags"))
	FGameplayTagContainer RequirementsTags;
};
