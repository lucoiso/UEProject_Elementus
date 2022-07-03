// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "ElementusInventoryData.generated.h"

UENUM(BlueprintType, Category = "Elementus Inventory | Enumerations")
enum class EElementusItemType : uint8
{
	None,
	Consumable,
	Armor,
	Weapon,
	Accessory,
	Crafting,
	Material,
	Information,
	Special,
	Event,
	Quest,
	Junk,
	Other
};

UCLASS(NotBlueprintable, NotPlaceable, Category = "Elementus Inventory | Classes | Data")
class ELEMENTUSINVENTORY_API UInventoryItemData final : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	explicit UInventoryItemData(const FObjectInitializer& ObjectInitializer);

	FORCEINLINE virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(*("ElementusInventoryItem_" + FString::FromInt(ItemId)));
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Elementus Inventory")
	int32 ItemId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Elementus Inventory")
	FName ItemName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Elementus Inventory")
	FText ItemDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Elementus Inventory")
	EElementusItemType ItemType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Elementus Inventory")
	float ItemValue;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Elementus Inventory", meta = (UIMin = 0, ClampMin = 0))
	float ItemWeight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Elementus Inventory")
	TSoftObjectPtr<UTexture2D> ItemIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Elementus Inventory")
	TSoftObjectPtr<UTexture2D> ItemImage;
};

USTRUCT(BlueprintType, Category = "Elementus Inventory | Structs | Data")
struct FElementusItemRowData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data", meta = (DisplayAfter = "Id"))
	TSoftObjectPtr<UInventoryItemData> Data;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data")
	FName Id;

	virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName) override
	{
		Super::OnDataTableChanged(InDataTable, InRowName);

		Id = Data ? *FString::FromInt(Data.LoadSynchronous()->ItemId) : TEXT("Undefined");
	}
};

USTRUCT(BlueprintType, Category = "Elementus Inventory | Structs | Data")
struct FElementusItemInfo
{
	GENERATED_USTRUCT_BODY()

	FElementusItemInfo() = default;

	explicit FElementusItemInfo(UInventoryItemData* ItemData, const int Quantity = 1)
		: ItemData(ItemData), ItemQuantity(Quantity)
	{
	}

	UPROPERTY(BlueprintReadWrite, Category = "Elementus Inventory")
	TObjectPtr<UInventoryItemData> ItemData;

	UPROPERTY(BlueprintReadWrite, Category = "Elementus Inventory", meta = (UIMin = 0, ClampMin = 0))
	int32 ItemQuantity;
};
