// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "ElementusInventoryData.h"
#include "Components/ActorComponent.h"
#include "ElementusInventoryComponent.generated.h"

UENUM(BlueprintType)
enum class EElementusInventoryUpdateChange : uint8
{
	Add,
	Remove
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FElementusInventoryUpdate, struct FElementusItemInfo, InventoryData,
                                             const EElementusInventoryUpdateChange, ChangeMode);

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

	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	void DebugInventoryStack();

private:
	UFUNCTION(NetMulticast, Reliable)
	void AddElementusItem_Internal(UInventoryItemData* ItemData,
	                               const int32 Quantity);

	UFUNCTION(NetMulticast, Reliable)
	void DiscardElementusItem_Internal(const UInventoryItemData* ItemData,
	                                   const int32 Quantity,
	                                   const bool bDropItem = false);
};
