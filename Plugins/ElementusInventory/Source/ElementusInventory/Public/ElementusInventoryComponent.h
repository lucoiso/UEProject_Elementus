// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "ElementusInventoryData.h"
#include "Components/ActorComponent.h"
#include "ElementusInventoryComponent.generated.h"

UCLASS(Blueprintable, ClassGroup=(Custom), meta =(BlueprintSpawnableComponent),
	Category = "Project Elementus | Classes")
class ELEMENTUSINVENTORY_API UElementusInventoryComponent final : public UActorComponent
{
	GENERATED_BODY()

public:
	UElementusInventoryComponent();

	UFUNCTION(BlueprintPure, Category = "Project Elementus | Inventory")
	TArray<FElementusItemInfo> GetItemStack();

	UFUNCTION(BlueprintPure, Category = "Project Elementus | Inventory")
	float GetCurrentWeight() const;

	UPROPERTY(BlueprintReadWrite, Category = "Elementus Inventory")
	float MaxWeight;

	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Inventory")
	void AddItemByData(UInventoryItemData* ItemData, const int32 Quantity);

	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Inventory")
	void AddItemById(const int32 ItemId, const int32 Quantity, UDataTable* InDataTable);

	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Inventory")
	void DiscardItemByData(const UInventoryItemData* ItemData, const int32 Quantity, const bool bDropItem = false);

	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Inventory")
	void DiscardItemById(const int32 ItemId, const int32 Quantity, const bool bDropItem = false);

protected:
	UPROPERTY(BlueprintReadWrite, Category = "Elementus Inventory")
	TArray<FElementusItemInfo> ItemStack;

	float CurrentWeight;
};
