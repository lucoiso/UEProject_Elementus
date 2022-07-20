// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "PEAbilityData.generated.h"

class UGameplayEffect;
class UGameplayAbility;

/**
 *
 */
USTRUCT(BlueprintType, Category = "Project Elementus | Structs")
struct FGameplayEffectGroupedData
{
	GENERATED_USTRUCT_BODY()

	FGameplayEffectGroupedData()
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Project Elementus | Properties")
	TSubclassOf<UGameplayEffect> EffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Project Elementus | Properties")
	TMap<FGameplayTag, float> SetByCallerStackedData;
};

/**
 *
 */
UCLASS(NotBlueprintable, NotPlaceable, Category = "Project Elementus | Classes")
class PROJECTELEMENTUS_API UPEAbilityData final : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	explicit UPEAbilityData(const FObjectInitializer& ObjectInitializer);

	FORCEINLINE virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(TEXT("PE_AbilityData"),
		                       *("Ability_" + FString::FromInt(AbilityId)));
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties",
		meta = (AssetBundles = "Data"))
	int32 AbilityId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties",
		meta = (AssetBundles = "Class"))
	TSoftClassPtr<UGameplayAbility> AbilityClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties",
		meta = (AssetBundles = "Data"))
	FName AbilityName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties",
		meta = (AssetBundles = "Data"))
	FText AbilityDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties",
		meta = (AssetBundles = "UI"))
	TSoftObjectPtr<UTexture2D> AbilityImage;
};
