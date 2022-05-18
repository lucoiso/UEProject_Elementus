// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GAS/System/PEAbilityData.h"
#include "Engine/DataAsset.h"
#include "PEConsumableData.generated.h"

class UGameplayEffect;
class UNiagaraSystem;
/**
 *
 */
UCLASS(NotBlueprintable, NotPlaceable, Category = "Project Elementus | Classes | Data")
class PROJECTELEMENTUS_API UPEConsumableData final : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	explicit UPEConsumableData(const FObjectInitializer& ObjectInitializer);

	FORCEINLINE virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("Consumable Data", GetFName());
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties")
	TSoftObjectPtr<UStaticMesh> ObjectMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties")
	TSoftObjectPtr<UNiagaraSystem> ObjectVFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties")
	TArray<FGameplayEffectGroupedData> ConsumableEffects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties")
	FGameplayTagContainer RequirementsTags;
};
