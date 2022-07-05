// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "ElementusInventoryComponent.h"

#include "ElementusInventoryFunctions.h"

UElementusInventoryComponent::UElementusInventoryComponent()
{
}

TArray<FElementusItemInfo> UElementusInventoryComponent::GetItemStack()
{
	return ItemStack;
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
	if (int ItemIndex = -1;
		UElementusInventoryFunctions::GetItemInfoByData(ItemData, ItemStack, ItemIndex))
	{
		ItemStack[ItemIndex].ItemQuantity += Quantity;
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Elementus Inventory - %s: Item not found in inventory."), *FString(__func__));
		ItemStack.Add(FElementusItemInfo(ItemData, Quantity));
	}
}

void UElementusInventoryComponent::AddItemById(const int32 ItemId, const int32 Quantity)
{
	if (int ItemIndex = -1;
		UElementusInventoryFunctions::GetItemInfoById(ItemId, ItemStack, ItemIndex))
	{
		ItemStack[ItemIndex].ItemQuantity += Quantity;
	}
	else if (UInventoryItemData* ReturnedItem =
		UElementusInventoryFunctions::GetUniqueElementusItemById(FString::FromInt(ItemId)))
	{
		UE_LOG(LogTemp, Display, TEXT("Elementus Inventory - %s: Item not found in inventory."), *FString(__func__));
		ItemStack.Add(FElementusItemInfo(ReturnedItem, Quantity));
	}
	else
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("Elementus Inventory - %s: Item with id %d not found!"),
		       *FString(__func__), ItemId);
	}
}

void UElementusInventoryComponent::DiscardItemByData(const UInventoryItemData* ItemData, const int32 Quantity,
                                                     const bool bDropItem)
{
	if (int ItemIndex = -1;
		UElementusInventoryFunctions::GetItemInfoByData(ItemData, ItemStack, ItemIndex))
	{
		ItemStack[ItemIndex].ItemQuantity += Quantity;

		if (Quantity >= ItemStack[ItemIndex].ItemQuantity)
		{
			ItemStack.RemoveAt(ItemIndex);
		}
		else
		{
			ItemStack[ItemIndex].ItemQuantity -= Quantity;
		}

		if (bDropItem)
		{
			// TO DO: Spawn a ElementusInventoryPackage with the items
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("Elementus Inventory - %s: Item with given data not found!"), *FString(__func__));
	}
}

void UElementusInventoryComponent::DiscardItemById(const int32 ItemId, const int32 Quantity, const bool bDropItem)
{
	if (int ItemIndex = -1;
		UElementusInventoryFunctions::GetItemInfoById(ItemId, ItemStack, ItemIndex))
	{
		ItemStack[ItemIndex].ItemQuantity += Quantity;

		if (Quantity >= ItemStack[ItemIndex].ItemQuantity)
		{
			ItemStack.RemoveAt(ItemIndex);
		}
		else
		{
			ItemStack[ItemIndex].ItemQuantity -= Quantity;
		}

		if (bDropItem)
		{
			// TO DO: Spawn a ElementusInventoryPackage with the items
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("Elementus Inventory - %s: Item with given data not found!"), *FString(__func__));
	}
}
