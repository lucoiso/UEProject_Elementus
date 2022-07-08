// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "ElementusInventoryData.h"
#include "Components/ActorComponent.h"
#include "ElementusInventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FElementusInventoryUpdate, struct FElementusItemInfo, InventoryData);

UCLASS(Blueprintable, ClassGroup=(Custom), meta =(BlueprintSpawnableComponent),
	Category = "Project Elementus | Classes")
class ELEMENTUSINVENTORY_API UElementusInventoryComponent final : public UActorComponent
{
	GENERATED_BODY()

public:
	UElementusInventoryComponent();

	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	float GetCurrentWeight() const;

	UPROPERTY(BlueprintReadWrite, Category = "Elementus Inventory")
	float MaxWeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elementus Inventory")
	TArray<FElementusItemInfo> ItemStack;

	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	void DebugInventoryStack();

	UPROPERTY(BlueprintAssignable, Category = "Elementus Inventory")
	FElementusInventoryUpdate OnInventoryUpdate;

	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	void AddItemByData(UInventoryItemData* ItemData, const int32 Quantity);

	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	void AddItemById(const int32 ItemId, const int32 Quantity);

	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	void DiscardItemByData(const UInventoryItemData* ItemData, const int32 Quantity, const bool bDropItem = false);

	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	void DiscardItemById(const int32 ItemId, const int32 Quantity, const bool bDropItem = false);

private:
	void AddItem_Worker(UInventoryItemData* ItemData, const int32 Quantity);
	void DiscardItem_Worker(const UInventoryItemData* ItemData, const int32 Quantity, const bool bDropItem = false);
};
