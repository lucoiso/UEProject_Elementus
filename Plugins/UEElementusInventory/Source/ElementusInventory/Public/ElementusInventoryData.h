// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEElementusInventory

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "ElementusInventoryData.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogElementusInventory, Display, Verbose);

DECLARE_LOG_CATEGORY_EXTERN(LogElementusInventory_Internal, Display, Verbose);

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
	Other,

	MAX
};

class UTexture2D;

UCLASS(NotBlueprintable, NotPlaceable, Category = "Elementus Inventory | Classes | Data")
class ELEMENTUSINVENTORY_API UElementusItemData final : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	explicit UElementusItemData(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	FORCEINLINE virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(TEXT("ElementusInventory_ItemData"), *("Item_" + FString::FromInt(ItemId)));
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Elementus Inventory", meta = (AssetBundles = "Data"))
	int32 ItemId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Elementus Inventory", meta = (AssetBundles = "SoftData"))
	TSoftObjectPtr<UObject> ItemObject;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Elementus Inventory", meta = (AssetBundles = "SoftData"))
	TSoftClassPtr<UObject> ItemClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Elementus Inventory", meta = (AssetBundles = "Data"))
	FName ItemName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Elementus Inventory", meta = (AssetBundles = "Data", MultiLine = "true"))
	FText ItemDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Elementus Inventory", meta = (AssetBundles = "Data"))
	EElementusItemType ItemType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Elementus Inventory", meta = (AssetBundles = "Data"))
	bool bIsStackable = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Elementus Inventory", meta = (AssetBundles = "Data"))
	float ItemValue;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Elementus Inventory", meta = (UIMin = 0, ClampMin = 0, AssetBundles = "Data"))
	float ItemWeight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Elementus Inventory", meta = (AssetBundles = "UI"))
	TSoftObjectPtr<UTexture2D> ItemIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Elementus Inventory", meta = (AssetBundles = "UI"))
	TSoftObjectPtr<UTexture2D> ItemImage;
};

USTRUCT(BlueprintType, Category = "Elementus Inventory | Structs")
struct FPrimaryElementusItemId : public FPrimaryAssetId
{
	GENERATED_USTRUCT_BODY()

	FPrimaryElementusItemId() : Super()
	{
	}

	explicit FPrimaryElementusItemId(const FPrimaryAssetId& InId) : Super(InId.PrimaryAssetType, InId.PrimaryAssetName)
	{
	}

	explicit FPrimaryElementusItemId(const FString& TypeAndName) : Super(TypeAndName)
	{
	}
};

USTRUCT(BlueprintType, Category = "Elementus Inventory | Structs")
struct FElementusItemInfo
{
	GENERATED_USTRUCT_BODY()

	FElementusItemInfo() = default;

	explicit FElementusItemInfo(const FPrimaryElementusItemId& InItemId) : ItemId(InItemId)
	{
	}

	explicit FElementusItemInfo(const FPrimaryElementusItemId& InItemId, const int32& InQuant) : ItemId(InItemId), Quantity(InQuant)
	{
	}

	explicit FElementusItemInfo(const FPrimaryElementusItemId& InItemId, const int32& InQuant, const FGameplayTagContainer& InTags) : ItemId(InItemId), Quantity(InQuant), Tags(InTags)
	{
	}

	bool operator==(const FElementusItemInfo& Other) const
	{
		return ItemId == Other.ItemId && Tags == Other.Tags;
	}

	bool operator!=(const FElementusItemInfo& Other) const
	{
		return !(ItemId == Other.ItemId && Tags == Other.Tags);
	}

	bool operator<(const FElementusItemInfo& Other) const
	{
		return ItemId.ToString() < Other.ItemId.ToString();
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Elementus Inventory")
	FPrimaryElementusItemId ItemId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elementus Inventory")
	int32 Quantity = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elementus Inventory")
	FGameplayTagContainer Tags;
};
