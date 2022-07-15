// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "ElementusInventoryData.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogElementusInventory, Display, Verbose);

DECLARE_LOG_CATEGORY_EXTERN(LogElementusInventory_Internal, Display, NoLogging);

constexpr auto& ElementusItemDataType = TEXT("ElementusInventory_ItemData");

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
		meta = (AssetBundles = "Data", MultiLine = "true"))
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
