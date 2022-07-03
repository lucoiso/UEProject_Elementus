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
UCLASS(NotBlueprintable, NotPlaceable, Category = "Project Elementus | Classes | Data")
class PROJECTELEMENTUS_API UPEConsumableData final : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	explicit UPEConsumableData(const FObjectInitializer& ObjectInitializer);

	FORCEINLINE virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(*("Consumable_" + FString::FromInt(ConsumableId)));
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties")
	int32 ConsumableId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties")
	TSoftObjectPtr<UStaticMesh> ObjectMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties")
	TSoftObjectPtr<UNiagaraSystem> ObjectVFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties")
	TArray<FGameplayEffectGroupedData> ConsumableEffects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties")
	FGameplayTagContainer RequirementsTags;
};

USTRUCT(BlueprintType, Category = "Project Elementus | Structs | Data")
struct FPEConsumableRowData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data", meta = (DisplayAfter = "Id"))
	TSoftObjectPtr<UPEConsumableData> Data;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data")
	FName Id;

	virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName) override
	{
		Super::OnDataTableChanged(InDataTable, InRowName);

		Id = Data ? *FString::FromInt(Data.LoadSynchronous()->ConsumableId) : TEXT("Undefined");
	}
};
