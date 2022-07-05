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

/**
 * 
 */
UCLASS(Category = "Elementus Inventory | Classes | Functions")
class ELEMENTUSINVENTORY_API UElementusInventoryFunctions final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Inventory")
	static bool GetItemInfoByData(const UInventoryItemData* InData, TArray<FElementusItemInfo> InArr, int& ItemIndex);

	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Inventory")
	static bool GetItemInfoById(const int32 InId, TArray<FElementusItemInfo> InArr, int& ItemIndex);

	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Inventory")
	static UInventoryItemData* GetUniqueElementusItemById(const FString InID);

	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Inventory")
	static TArray<UInventoryItemData*> SearchElementusItem(const EElementusSearchType SearchType,
	                                                       const FString SearchString);
};
