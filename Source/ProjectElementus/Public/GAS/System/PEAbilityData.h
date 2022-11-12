// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "PEAbilityData.generated.h"

class UGameplayAbility;

/**
 *
 */
UCLASS(NotBlueprintable, NotPlaceable, Category = "Project Elementus | Classes")
class PROJECTELEMENTUS_API UPEAbilityData final : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	explicit UPEAbilityData(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	FORCEINLINE virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(TEXT("PE_AbilityData"), *("Ability_" + FString::FromInt(AbilityId)));
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus", meta = (AssetBundles = "Data"))
	int32 AbilityId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus", meta = (AssetBundles = "Class"))
	TSoftClassPtr<UGameplayAbility> AbilityClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus", meta = (AssetBundles = "Data"))
	FName AbilityName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus", meta = (AssetBundles = "Data"))
	FText AbilityDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus", meta = (AssetBundles = "UI"))
	TSoftObjectPtr<UTexture2D> AbilityImage;
};
