// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Components/PEInventoryComponent.h"
#include "Management/Data/PEGlobalTags.h"
#include "ElementusInventoryFunctions.h"
#include "Actors/Interfaces/PEEquipment.h"
#include "Actors/Character/PECharacter.h"

UPEInventoryComponent::UPEInventoryComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

bool UPEInventoryComponent::CanGiveItem(const FElementusItemInfo InItemInfo) const
{
	// We cannot give the item if it is currently equiped
	return Super::CanGiveItem(InItemInfo) && !InItemInfo.Tags.HasTag(GlobalTag_EquipSlot_Base);
}

bool UPEInventoryComponent::EquipItem(const FElementusItemInfo& InItem)
{	
	if (GetOwnerRole() != ROLE_Authority)
	{
		return false;
	}

	if (!IsValid(GetOwner()))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s - Invalid owning actor"), *FString(__func__));
		return false;
	}

	if (!ContainsItem(InItem))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s - The item %s is not in the %s's inventory"), *FString(__func__), *InItem.ItemId.ToString(), *GetOwner()->GetName());
		return false;
	}

	if (!GetOwner()->GetClass()->IsChildOf<APECharacter>())
	{
		UE_LOG(LogTemp, Warning, TEXT("%s - Owning actor isn't child of PECharacter"), *FString(__func__));
		return false;
	}

	if (const UElementusItemData* const ItemData = UElementusInventoryFunctions::GetSingleItemDataById(InItem.ItemId, { "SoftData" }, false))
	{
		if (UPEEquipment* const EquipedItem = Cast<UPEEquipment>(ItemData->ItemClass.LoadSynchronous()->GetDefaultObject()))
		{
			const FGameplayTagContainer EquipmentSlotTags = EquipedItem->EquipmentSlotTags;
			if (int32 FoundIndex;
				FindFirstItemIndexWithTags(EquipmentSlotTags, FoundIndex))
			{
				// Already equipped
				UE_LOG(LogTemp, Display, TEXT("%s - Actor %s has already unequipped item %s"), *FString(__func__), *GetOwner()->GetName(), *InItem.ItemId.ToString());
				
				UnnequipItem(GetItemReferenceAt(FoundIndex));
				return false;
			}

			if (int32 FoundIndex;
				FindFirstItemIndexWithInfo(InItem, FoundIndex))
			{
				if (!EquipedItem->ProcessEquipmentApplication(Cast<APECharacter>(GetOwner())))
				{
					UElementusInventoryFunctions::UnloadElementusItem(InItem.ItemId);
					return false;
				}

				for (const FGameplayTag& Iterator : EquipmentSlotTags)
				{
					EquipmentMap.Add(Iterator, InItem);
				}				
				GetItemReferenceAt(FoundIndex).Tags.AppendTags(EquipmentSlotTags);

				UE_LOG(LogTemp, Display, TEXT("%s - Actor %s equipped %s"), *FString(__func__), *GetOwner()->GetName(), *InItem.ItemId.ToString());
				OnInventoryUpdate.Broadcast();				

				UElementusInventoryFunctions::UnloadElementusItem(InItem.ItemId);
				return true;
			}
		}
	}

	UElementusInventoryFunctions::UnloadElementusItem(InItem.ItemId);
	return false;
}

bool UPEInventoryComponent::UnnequipItem(FElementusItemInfo& InItem)
{
	if (GetOwnerRole() != ROLE_Authority)
	{
		return false;
	}

	if (!IsValid(GetOwner()))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s - Invalid owning actor"), *FString(__func__));
		return false;
	}

	if (!ContainsItem(InItem))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s - The item %s is not in the %s's inventory"), *FString(__func__), *InItem.ItemId.ToString(), *GetOwner()->GetName());
		return false;
	}

	if (!GetOwner()->GetClass()->IsChildOf<APECharacter>())
	{
		UE_LOG(LogTemp, Warning, TEXT("%s - Owning actor isn't child of PECharacter"), *FString(__func__));
		return false;
	}

	if (const UElementusItemData* const ItemData = UElementusInventoryFunctions::GetSingleItemDataById(InItem.ItemId, { "SoftData" }, false))
	{
		if (UPEEquipment* const EquipedItem = Cast<UPEEquipment>(ItemData->ItemClass.LoadSynchronous()->GetDefaultObject()))
		{
			if (!EquipedItem->ProcessEquipmentRemoval(Cast<APECharacter>(GetOwner())))
			{
				UElementusInventoryFunctions::UnloadElementusItem(InItem.ItemId);
				return false;
			}
			
			const FGameplayTagContainer EquipmentSlotTags = EquipedItem->EquipmentSlotTags;			
			for (const FGameplayTag& Iterator : EquipmentSlotTags)
			{
				EquipmentMap.Remove(Iterator);
			}			
			InItem.Tags.RemoveTags(EquipmentSlotTags);

			UE_LOG(LogTemp, Display, TEXT("%s - Actor %s unequipped %s"), *FString(__func__), *GetOwner()->GetName(), *InItem.ItemId.ToString());
			OnInventoryUpdate.Broadcast();

			UElementusInventoryFunctions::UnloadElementusItem(InItem.ItemId);
			return true;
		}
	}

	UElementusInventoryFunctions::UnloadElementusItem(InItem.ItemId);
	return false;
}
