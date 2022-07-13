// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

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
/**
 * 
 */
UCLASS(Category = "Elementus Inventory | Classes | Functions")
class ELEMENTUSINVENTORY_API UElementusInventoryFunctions final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	static bool CompareItemInfoIds(const FPrimaryAssetId& Info1, const FPrimaryAssetId& Info2);

	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	static bool CompareItemDataIds(const UInventoryItemData* Data1, const UInventoryItemData* Data2);

	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	static UInventoryItemData* GetElementusItemDataById(const FPrimaryAssetId& InID,
	                                                    const TArray<FName>& InBundles);

	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	static TArray<UInventoryItemData*> GetElementusItemDataArrayById(const TArray<FPrimaryAssetId> InIDs,
	                                                                 const TArray<FName>& InBundles);

	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	static TArray<UInventoryItemData*> SearchElementusItemData(const EElementusSearchType SearchType,
	                                                           const FString& SearchString,
	                                                           const TArray<FName>& InBundles);

	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	static TArray<FPrimaryAssetId> GetElementusItemIds();

	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	static void TradeElementusItem(TMap<FPrimaryAssetId, int32> ItemsToTrade,
	                               UElementusInventoryComponent* FromInventory,
	                               UElementusInventoryComponent* ToInventory);

private:
	static TArray<UInventoryItemData*> LoadElementusItemDatas_Internal(UAssetManager* InAssetManager,
	                                                                   const TArray<FPrimaryAssetId> InIDs,
	                                                                   const TArray<FName>& InBundles);
};
