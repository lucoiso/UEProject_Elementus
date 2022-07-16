// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "ElementusInventoryComponent.h"
#include "ElementusInventoryFunctions.h"
#include "Engine/AssetManager.h"

#if WITH_EDITOR
#include "Misc/MessageDialog.h"
#endif

UElementusInventoryComponent::UElementusInventoryComponent()
	: CurrentWeight(0.f)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

float UElementusInventoryComponent::GetCurrentWeight() const
{
	return CurrentWeight;
}

TMap<FPrimaryAssetId, int32> UElementusInventoryComponent::GetItemStack() const
{
	return ItemStack;
}

void UElementusInventoryComponent::AddElementusItem(const FPrimaryAssetId& ItemId, const int32 Quantity)
{
	UE_LOG(LogElementusInventory, Display,
	       TEXT("Elementus Inventory - %s: Adding %d item(s) with name '%s' to inventory"),
	       *FString(__func__), Quantity, *ItemId.ToString());

	AddElementusItem_Internal(ItemId, Quantity);
}

void UElementusInventoryComponent::DiscardElementusItem(const FPrimaryAssetId& ItemId, const int32 Quantity)
{
	UE_LOG(LogElementusInventory, Display,
	       TEXT("Elementus Inventory - %s: Discarding %d item(s) with name '%s' from inventory"),
	       *FString(__func__), Quantity, *ItemId.ToString());

	DiscardElementusItem_Internal(ItemId, Quantity);
}

constexpr void DoMulticastLoggingIdentification(const ENetMode& CurrentNetMode)
{
	if (CurrentNetMode == NM_Client)
	{
		UE_LOG(LogElementusInventory_Internal, Warning, TEXT("Elementus Inventory - %s: Client logging: "),
		       *FString(__func__));
	}
	else if (CurrentNetMode != NM_Standalone)
	{
		UE_LOG(LogElementusInventory_Internal, Warning, TEXT("Elementus Inventory - %s: Server logging: "),
		       *FString(__func__));
	}
}

void UElementusInventoryComponent::AddElementusItem_Internal_Implementation(const FPrimaryAssetId& ItemId,
                                                                            const int32 Quantity)
{
	DoMulticastLoggingIdentification(GetOwner()->GetNetMode());

	UE_LOG(LogElementusInventory_Internal, Display,
	       TEXT("Elementus Inventory - %s: Adding %d item(s) with name '%s' to inventory"),
	       *FString(__func__), Quantity, *ItemId.ToString());

	if (ItemStack.Contains(ItemId))
	{
		*ItemStack.Find(ItemId) += Quantity;
	}
	else
	{
		ItemStack.Add(ItemId, Quantity);
	}

	NotifyInventoryChange(ItemId, Quantity);
}

void UElementusInventoryComponent::DiscardElementusItem_Internal_Implementation(const FPrimaryAssetId& ItemId,
	const int32 Quantity)
{
	DoMulticastLoggingIdentification(GetOwner()->GetNetMode());

	UE_LOG(LogElementusInventory_Internal, Display,
	       TEXT("Elementus Inventory - %s: Discarding %d item(s) with name '%s' from inventory"),
	       *FString(__func__), Quantity, *ItemId.ToString());

	if (auto CurrentItemQuant = ItemStack.FindRef(ItemId))
	{
		CurrentItemQuant =
			FMath::Clamp<int32>(CurrentItemQuant - Quantity,
			                    0,
			                    CurrentItemQuant);

		if (CurrentItemQuant == 0)
		{
			ItemStack.Remove(ItemId);
		}
		else
		{
			*ItemStack.Find(ItemId) = CurrentItemQuant;
		}

		NotifyInventoryChange(ItemId, CurrentItemQuant);
	}
}

void UElementusInventoryComponent::DebugInventoryStack()
{
	UE_LOG(LogElementusInventory, Warning, TEXT("Elementus Inventory - %s"), *FString(__func__));
	UE_LOG(LogElementusInventory, Warning, TEXT("Owning Actor: %s"), *GetOwner()->GetName());

	for (const auto& Iterator : ItemStack)
	{
		UE_LOG(LogElementusInventory, Warning, TEXT("Item: %s"), *Iterator.Key.ToString());
		UE_LOG(LogElementusInventory, Warning, TEXT("Quantity: %d"), Iterator.Value);
	}

	UE_LOG(LogElementusInventory, Warning, TEXT("Weight: %d"), CurrentWeight);
}

bool UElementusInventoryComponent::CanReceiveItem(const FPrimaryAssetId& ItemId, const int32 Quantity) const
{
	if (const UInventoryItemData* ItemData =
		UElementusInventoryFunctions::GetElementusItemDataById(ItemId, {"Data"}))
	{
		if (CurrentWeight + ItemData->ItemWeight * Quantity <= MaxWeight)
		{
			return true;
		}
	}

	return false;
}

bool UElementusInventoryComponent::CanGiveItem(const FPrimaryAssetId& ItemId, const int32 Quantity) const
{
	return ItemStack.Contains(ItemId) && ItemStack.FindRef(ItemId) >= Quantity;
}

void UElementusInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	UpdateCurrentWeight();
	ValidateItemStack();
}

#if WITH_EDITOR
void UElementusInventoryComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property == FindFieldChecked<FProperty>(GetClass(), "ItemStack"))
	{
		ValidateItemStack();
	}
}
#endif

void UElementusInventoryComponent::NotifyInventoryChange(const FPrimaryAssetId& ItemId, const int32 Quantity)
{
	OnInventoryUpdate.Broadcast(ItemId, ItemStack.Contains(ItemId) ? ItemStack.FindRef(ItemId) : 0);

	if (const UInventoryItemData* ItemData =
			UElementusInventoryFunctions::GetElementusItemDataById(ItemId, {"Data"});
		ItemStack.Contains(ItemId))
	{
		CurrentWeight += ItemData->ItemWeight * Quantity;
	}
	else
	{
		UpdateCurrentWeight();
	}
}

void UElementusInventoryComponent::UpdateCurrentWeight()
{
	TArray<FPrimaryAssetId> ItemIds;
	ItemStack.GetKeys(ItemIds);

	float NewWeigth = 0.f;
	if (const TArray<UInventoryItemData*>& ItemDataArr =
			UElementusInventoryFunctions::GetElementusItemDataArrayById(ItemIds, {"Data"});
		!ItemDataArr.IsEmpty())
	{
		for (const auto& Iterator : ItemDataArr)
		{
			if (ItemStack.Contains(Iterator->GetPrimaryAssetId()))
			{
				NewWeigth += Iterator->ItemWeight * ItemStack.FindRef(Iterator->GetPrimaryAssetId());
			}
		}
	}

	CurrentWeight = NewWeigth;
}

void UElementusInventoryComponent::ValidateItemStack()
{
	TArray<FPrimaryAssetId> ItemIds;
	ItemStack.GetKeys(ItemIds);

	bool bHasInvalidItems = false;
	const bool bEnableLogging = GetOwner()->IsTemplate()
		|| GetOwner()->GetWorld()->IsGameWorld()
#if WITH_EDITOR
		|| GetOwner()->IsSelectedInEditor();
#else
		|| false; // just to add the ';' to the end of the line
#endif

	for (const auto& Iterator : ItemIds)
	{
		if (Iterator.PrimaryAssetType.IsValid()
			&& Iterator.PrimaryAssetType != FPrimaryAssetType(ElementusItemDataType))
		{
			if (bEnableLogging)
			{
				UE_LOG(LogElementusInventory, Error,
				       TEXT("Elementus Inventory - %s: Removing item %s from %s inventory"),
				       *FString(__func__), *Iterator.ToString(), *GetOwner()->GetName());
			}

			ItemStack.Remove(Iterator);
			bHasInvalidItems = true;
		}
	}

	if (bHasInvalidItems && bEnableLogging)
	{
		UE_LOG(LogElementusInventory, Error,
		       TEXT(
			       "Elementus Inventory - %s: Elementus Inventory only supports Item Data Assets derived from UInventoryItemData"
		       ),
		       *FString(__func__));

#if WITH_EDITOR
		FMessageDialog::Open(EAppMsgType::Ok,
		                     FText::FromString(
			                     "Elementus Inventory only supports Item Data Assets derived from UInventoryItemData."));
#endif
	}
}
