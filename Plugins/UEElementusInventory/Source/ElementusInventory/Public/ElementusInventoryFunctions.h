// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEElementusInventory

#pragma once

#include "CoreMinimal.h"
#include "ElementusInventoryData.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ElementusInventoryFunctions.generated.h"

UENUM(BlueprintType, Category = "Elementus Inventory | Enumerations")
enum class EElementusSearchType : uint8
{
	Name,
	ID,
	Type
};

class UElementusInventoryComponent;
class UAssetManager;
struct FPrimaryElementusItemId;
/**
 * 
 */
UCLASS(Category = "Elementus Inventory | Functions")
class ELEMENTUSINVENTORY_API UElementusInventoryFunctions final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/* Unload all elementus items that were loaded by Asset Manager */
	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	static void UnloadAllElementusItems();

	/* Unload a elementus item that was loaded by Asset Manager */
	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	static void UnloadElementusItem(const FPrimaryElementusItemId& InItemId);

	/* Check if the ids are equal */
	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	static bool CompareItemInfo(const FElementusItemInfo& Info1, const FElementusItemInfo& Info2);

	/* Check if the id of the passed datas are equal */
	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	static bool CompareItemData(const UElementusItemData* Data1, const UElementusItemData* Data2);

	/* Return the data of the passed Id */
	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	static UElementusItemData* GetSingleItemDataById(const FPrimaryElementusItemId& InID, const TArray<FName>& InBundles, const bool bAutoUnload = true);

	/* Return a array of data depending of the given ids */
	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	static TArray<UElementusItemData*> GetItemDataArrayById(const TArray<FPrimaryElementusItemId>& InIDs, const TArray<FName>& InBundles, const bool bAutoUnload = true);

	/* Search items and return a array of item data */
	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	static TArray<UElementusItemData*> SearchItemData(const EElementusSearchType SearchType, const FString& SearchString, const TArray<FName>& InBundles, const bool bAutoUnload = true);

	/* Get ids of all registered items */
	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	static TArray<FPrimaryElementusItemId> GetAllElementusItemIds();

	/* Trade items between two inventory components */
	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	static void TradeElementusItem(TArray<FElementusItemInfo> ItemsToTrade, UElementusInventoryComponent* FromInventory, UElementusInventoryComponent* ToInventory);

	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	static bool IsItemValid(const FElementusItemInfo InItemInfo);

	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	static bool IsItemStackable(const FElementusItemInfo InItemInfo);

	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	static FGameplayTagContainer GetItemTagsWithParentTag(const FElementusItemInfo InItemInfo, const FGameplayTag FromParentTag);

private:
	static TArray<UElementusItemData*> LoadElementusItemDatas_Internal(UAssetManager* InAssetManager, const TArray<FPrimaryElementusItemId>& InIDs, const TArray<FName>& InBundles, const bool bAutoUnload);
};
