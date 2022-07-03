// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "PEResourceData.generated.h"

/**
 *
 */
UCLASS(NotBlueprintable, NotPlaceable, Category = "Project Elementus | Classes | Data")
class PROJECTELEMENTUS_API UPEResourceData final : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	explicit UPEResourceData(const FObjectInitializer& ObjectInitializer);

	FORCEINLINE virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(*("Resource_" + FString::FromInt(ResourceId)));
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties")
	int32 ResourceId;
};

USTRUCT(BlueprintType, Category = "Project Elementus | Structs | Data")
struct FPEResourceRowData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data", meta = (DisplayAfter = "Id"))
	TSoftObjectPtr<UPEResourceData> Data;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data")
	FName Id;

	virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName) override
	{
		Super::OnDataTableChanged(InDataTable, InRowName);

		Id = Data ? *FString::FromInt(Data.LoadSynchronous()->ResourceId) : TEXT("Undefined");
	}
};
