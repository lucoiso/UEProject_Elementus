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
USTRUCT(BlueprintType, Category = "Project Elementus | Structs | Data")
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
UCLASS(NotBlueprintable, NotPlaceable, Category = "Project Elementus | Classes | Data")
class PROJECTELEMENTUS_API UPEAbilityData final : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	explicit UPEAbilityData(const FObjectInitializer& ObjectInitializer);

	FORCEINLINE virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(*("Ability_" + FString::FromInt(AbilityId)));
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties")
	int32 AbilityId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties")
	TSoftClassPtr<UGameplayAbility> AbilityClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties")
	FName AbilityName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties")
	FText AbilityDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties")
	TSoftObjectPtr<UTexture2D> AbilityImage;
};

USTRUCT(BlueprintType, Category = "Project Elementus | Structs | Data")
struct FPEAbilityRowData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data", meta = (DisplayAfter = "Id"))
	TSoftObjectPtr<UPEAbilityData> Data;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data")
	FName Id;

	virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName) override
	{
		Super::OnDataTableChanged(InDataTable, InRowName);

		Id = Data ? *FString::FromInt(Data.LoadSynchronous()->AbilityId) : TEXT("Undefined");
	}
};
