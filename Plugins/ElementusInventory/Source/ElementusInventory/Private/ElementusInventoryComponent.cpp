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
	AddElementusItem_Internal(ItemData, Quantity);
}

void UElementusInventoryComponent::AddItemById(const int32 ItemId, const int32 Quantity)
{
	if (UInventoryItemData* ReturnedItem =
		UElementusInventoryFunctions::GetElementusItemDataById(FString::FromInt(ItemId)))
	{
		AddElementusItem_Internal(ReturnedItem, Quantity);
	}
}

void UElementusInventoryComponent::DiscardItemByData(const UInventoryItemData* ItemData,
                                                     const int32 Quantity,
                                                     const bool bDropItem)
{
	DiscardElementusItem_Internal(ItemData, Quantity, bDropItem);
}

void UElementusInventoryComponent::DiscardItemById(const int32 ItemId,
                                                   const int32 Quantity,
                                                   const bool bDropItem)
{
	if (const UInventoryItemData* ReturnedItem =
		UElementusInventoryFunctions::GetElementusItemDataById(FString::FromInt(ItemId)))
	{
		DiscardElementusItem_Internal(ReturnedItem, Quantity, bDropItem);
	}
}

constexpr void DoMulticastLoggingIdentification(const ENetMode& CurrentNetMode)
{	
	if (CurrentNetMode == NM_Client)
	{		
		UE_LOG(LogElementusInventory, Warning, TEXT("Elementus Inventory - %s: Client logging: "), *FString(__func__));
	}
	else if (CurrentNetMode != NM_Standalone)
	{
		UE_LOG(LogElementusInventory, Warning, TEXT("Elementus Inventory - %s: Server logging: "), *FString(__func__));
	}
}

void UElementusInventoryComponent::AddElementusItem_Internal_Implementation(UInventoryItemData* ItemData,
                                                                            const int32 Quantity)
{
	DoMulticastLoggingIdentification(GetOwner()->GetNetMode());

	UE_LOG(LogElementusInventory, Display,
	       TEXT("Elementus Inventory - %s: Adding %d item(s) with name '%s' to inventory"),
	       *FString(__func__), Quantity, *ItemData->ItemName.ToString());

	if (int ItemIndex = -1;
		UElementusInventoryFunctions::FindElementusItemInfoByDataInArr(ItemData, ItemStack, ItemIndex))
	{
		ItemStack[ItemIndex].ItemQuantity += Quantity;
		OnInventoryUpdate.Broadcast(ItemStack[ItemIndex], EElementusInventoryUpdateChange::Add);
	}
	else
	{
		ItemStack.Add(FElementusItemInfo(ItemData, Quantity));
		OnInventoryUpdate.Broadcast(ItemStack.Last(), EElementusInventoryUpdateChange::Add);
	}
}

void UElementusInventoryComponent::DiscardElementusItem_Internal_Implementation(const UInventoryItemData* ItemData,
	const int32 Quantity,
	const bool bDropItem)
{
	DoMulticastLoggingIdentification(GetOwner()->GetNetMode());

	UE_LOG(LogElementusInventory, Display,
	       TEXT("Elementus Inventory - %s: Discarding %d item(s) with name '%s' from inventory"),
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

		OnInventoryUpdate.Broadcast(DiscardedItem, EElementusInventoryUpdateChange::Remove);

		if (bDropItem)
		{
			// TO DO: Spawn a ElementusInventoryPackage with the items
			// Note: The user needs to choose the class and the look of the package
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
