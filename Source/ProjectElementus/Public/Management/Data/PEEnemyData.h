// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "PEEnemyData.generated.h"

/**
 *
 */
UCLASS(NotBlueprintable, NotPlaceable, Category = "Project Elementus | Classes | Data")
class PROJECTELEMENTUS_API UPEEnemyData final : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	explicit UPEEnemyData(const FObjectInitializer& ObjectInitializer);

	FORCEINLINE virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(*("Enemy_" + FString::FromInt(EnemyId)));
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties")
	int32 EnemyId;
};

USTRUCT(BlueprintType, Category = "Project Elementus | Structs | Data")
struct FPEEnemyRowData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data", meta = (DisplayAfter = "Id"))
	TSoftObjectPtr<UPEEnemyData> Data;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data")
	FName Id;

	virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName) override
	{
		Super::OnDataTableChanged(InDataTable, InRowName);

		Id = Data ? *FString::FromInt(Data.LoadSynchronous()->EnemyId) : TEXT("Undefined");
	}
};
