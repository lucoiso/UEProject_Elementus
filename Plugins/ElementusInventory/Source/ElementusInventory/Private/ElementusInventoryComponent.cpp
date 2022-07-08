// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "ElementusInventoryComponent.h"
#include "ElementusInventoryFunctions.h"

UElementusInventoryComponent::UElementusInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

float UElementusInventoryComponent::GetCurrentWeight() const
{
	float OutputValue = 0.f;
	for (const auto& [ItemData, ItemQuantity] : ItemStack)
	{
		OutputValue += ItemData->ItemWeight * ItemQuantity;
	}

	return OutputValue;
}

void UElementusInventoryComponent::AddItemByData(UInventoryItemData* ItemData, const int32 Quantity)
{
	AddItem_Worker(ItemData, Quantity);
}

void UElementusInventoryComponent::AddItemById(const int32 ItemId, const int32 Quantity)
{
	if (UInventoryItemData* ReturnedItem =
		UElementusInventoryFunctions::GetElementusItemDataById(FString::FromInt(ItemId)))
	{
		AddItem_Worker(ReturnedItem, Quantity);
	}
}

void UElementusInventoryComponent::DiscardItemByData(const UInventoryItemData* ItemData,
                                                     const int32 Quantity,
                                                     const bool bDropItem)
{
	DiscardItem_Worker(ItemData, Quantity, bDropItem);
}

void UElementusInventoryComponent::DiscardItemById(const int32 ItemId,
                                                   const int32 Quantity,
                                                   const bool bDropItem)
{
	if (const UInventoryItemData* ReturnedItem =
		UElementusInventoryFunctions::GetElementusItemDataById(FString::FromInt(ItemId)))
	{
		DiscardItem_Worker(ReturnedItem, Quantity, bDropItem);
	}
}

void UElementusInventoryComponent::AddItem_Worker(UInventoryItemData* ItemData, const int32 Quantity)
{
	UE_LOG(LogElementusInventory, Display,
	       TEXT("Elementus Inventory - %s: Adding %d item with name '%s' to inventory"),
	       *FString(__func__), Quantity, *ItemData->ItemName.ToString());

	if (int ItemIndex = -1;
		UElementusInventoryFunctions::FindElementusItemInfoByDataInArr(ItemData, ItemStack, ItemIndex))
	{
		ItemStack[ItemIndex].ItemQuantity += Quantity;
		OnInventoryUpdate.Broadcast(ItemStack[ItemIndex]);
	}
	else
	{
		ItemStack.Add(FElementusItemInfo(ItemData, Quantity));
		OnInventoryUpdate.Broadcast(ItemStack.Last());
	}
}

void UElementusInventoryComponent::DiscardItem_Worker(const UInventoryItemData* ItemData, const int32 Quantity,
                                                      const bool bDropItem)
{
	UE_LOG(LogElementusInventory, Display,
	       TEXT("Elementus Inventory - %s: Discarding %d item with name '%s' from inventory"),
	       *FString(__func__), Quantity, *ItemData->ItemName.ToString());

	if (int ItemIndex = -1;
		UElementusInventoryFunctions::FindElementusItemInfoByDataInArr(ItemData, ItemStack, ItemIndex))
	{
		ItemStack[ItemIndex].ItemQuantity =
			FMath::Clamp<int32>(ItemStack[ItemIndex].ItemQuantity - Quantity,
			                    0,
			                    ItemStack[ItemIndex].ItemQuantity);

		const FElementusItemInfo DiscardedItem = ItemStack[ItemIndex];
		if (Quantity >= ItemStack[ItemIndex].ItemQuantity)
		{
			ItemStack.RemoveAt(ItemIndex);
		}

		OnInventoryUpdate.Broadcast(DiscardedItem);

		if (bDropItem)
		{
			// TO DO: Spawn a ElementusInventoryPackage with the items
		}
	}
}

void UElementusInventoryComponent::DebugInventoryStack()
{
	UE_LOG(LogElementusInventory, Warning, TEXT("Elementus Inventory - %s"), *FString(__func__));
	UE_LOG(LogElementusInventory, Warning, TEXT("Owning Actor: %s"), *GetOwner()->GetName());

	for (const auto& Iterator : ItemStack)
	{
		UE_LOG(LogElementusInventory, Warning, TEXT("Item: %s"), *Iterator.ItemData->ItemName.ToString());
		UE_LOG(LogElementusInventory, Warning, TEXT("Quantity: %d"), Iterator.ItemQuantity);
	}
}
