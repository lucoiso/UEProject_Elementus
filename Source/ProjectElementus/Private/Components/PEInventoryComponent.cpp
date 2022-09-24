// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Components/PEInventoryComponent.h"
#include "Management/Data/PEGlobalTags.h"

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
