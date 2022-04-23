// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "PEAbilityData.generated.h"

class UGameplayEffect;
class UGameplayAbility;

/**
 *
 */
USTRUCT(BlueprintType, Category = "Custom GAS | Data")
struct FGameplayEffectGroupedData
{
	GENERATED_USTRUCT_BODY()

public:
	FGameplayEffectGroupedData()
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom GAS | Defaults")
		TSubclassOf<UGameplayEffect> EffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom GAS | Defaults")
		TMap<FGameplayTag, float> SetByCallerStackedData;
};

/**
 *
 */
UCLASS(NotBlueprintable, NotPlaceable, Category = "Custom Data | Data Assets")
class PROJECTELEMENTUS_API UPEAbilityData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPEAbilityData(const FObjectInitializer& ObjectInitializer);

	FORCEINLINE virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("Ability Data", GetFName());
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom Properties | Defaults")
		TSoftClassPtr<UGameplayAbility> AbilityClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom Properties | Defaults")
		FName AbilityName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom Properties | Defaults")
		FText AbilityDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom Properties | Defaults")
		TSoftObjectPtr<UTexture2D> AbilityImage;
};