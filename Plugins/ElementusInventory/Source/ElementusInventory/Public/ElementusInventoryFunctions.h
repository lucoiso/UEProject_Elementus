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
	static bool CompareItemInfoIds(const FElementusItemInfo& Info1, const FElementusItemInfo& Info2);

	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	static bool CompareItemDataIds(const UInventoryItemData* Data1, const UInventoryItemData* Data2);

	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	static bool FindElementusItemInfoByDataInArr(const UInventoryItemData* InData, TArray<FElementusItemInfo> InArr,
	                                             int& ItemIndex);

	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	static bool FindElementusItemInfoByIdInArr(const int32 InId, TArray<FElementusItemInfo> InArr, int& ItemIndex);

	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	static UInventoryItemData* GetElementusItemDataById(const FString InID);

	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	static TArray<UInventoryItemData*> SearchElementusItemData(const EElementusSearchType SearchType,
	                                                           const FString SearchString);

	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	static void TradeElementusItem(FElementusItemInfo ItemInfo,
	                               UElementusInventoryComponent* FromInventory,
	                               UElementusInventoryComponent* ToInventory);
};
