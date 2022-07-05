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
		return FPrimaryAssetId(TEXT("ElementusInventory_ItemData"),
		                       *("Item_" + FString::FromInt(ItemId)));
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Elementus Inventory",
		meta = (AssetBundles = "Data"))
	int32 ItemId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Elementus Inventory",
		meta = (AssetBundles = "Actor"))
	TSoftClassPtr<UObject> ItemClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Elementus Inventory",
		meta = (AssetBundles = "Data"))
	FName ItemName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Elementus Inventory",
		meta = (AssetBundles = "Data"))
	FText ItemDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Elementus Inventory",
		meta = (AssetBundles = "Data"))
	EElementusItemType ItemType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Elementus Inventory",
		meta = (AssetBundles = "Data"))
	float ItemValue;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Elementus Inventory",
		meta = (UIMin = 0, ClampMin = 0, AssetBundles = "Data"))
	float ItemWeight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Elementus Inventory",
		meta = (AssetBundles = "UI"))
	TSoftObjectPtr<UTexture2D> ItemIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Elementus Inventory",
		meta = (AssetBundles = "UI"))
	TSoftObjectPtr<UTexture2D> ItemImage;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elementus Inventory")
	TObjectPtr<UInventoryItemData> ItemData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elementus Inventory",
		meta = (UIMin = 0, ClampMin = 0))
	int32 ItemQuantity;
};
