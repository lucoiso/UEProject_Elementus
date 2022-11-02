// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEElementusInventory

#include "ElementusInventoryFunctions.h"
#include "ElementusInventoryComponent.h"
#include "Engine/AssetManager.h"

void UElementusInventoryFunctions::UnloadAllElementusItems()
{
	if (UAssetManager* const AssetManager = UAssetManager::GetIfValid())
	{
		AssetManager->UnloadPrimaryAssetsWithType(FPrimaryAssetType(ElementusItemDataType));
	}
}

void UElementusInventoryFunctions::UnloadElementusItem(const FPrimaryElementusItemId& InItemId)
{
	if (UAssetManager* const AssetManager = UAssetManager::GetIfValid())
	{
		AssetManager->UnloadPrimaryAsset(FPrimaryAssetId(InItemId));
	}
}

bool UElementusInventoryFunctions::CompareItemInfo(const FElementusItemInfo& Info1, const FElementusItemInfo& Info2)
{
	return Info1 == Info2;
}

bool UElementusInventoryFunctions::CompareItemData(const UElementusItemData* Data1, const UElementusItemData* Data2)
{
	return Data1->GetPrimaryAssetId() == Data2->GetPrimaryAssetId();
}

UElementusItemData* UElementusInventoryFunctions::GetSingleItemDataById(const FPrimaryElementusItemId& InID, const TArray<FName>& InBundles, const bool bAutoUnload)
{
	UElementusItemData* Output = nullptr;
	if (UAssetManager* const AssetManager = UAssetManager::GetIfValid())
	{
		if (const TSharedPtr<FStreamableHandle> StreamableHandle = AssetManager->LoadPrimaryAsset(InID, InBundles);
			StreamableHandle.IsValid())
		{
			StreamableHandle->WaitUntilComplete(5.f);
			Output = Cast<UElementusItemData>(StreamableHandle->GetLoadedAsset());
		}
		else // The object is already loaded
		{
			Output = AssetManager->GetPrimaryAssetObject<UElementusItemData>(InID);
		}

		if (bAutoUnload)
		{
			AssetManager->UnloadPrimaryAsset(InID);
		}
	}

	return Output;
}

TArray<UElementusItemData*> UElementusInventoryFunctions::GetItemDataArrayById(const TArray<FPrimaryElementusItemId>& InIDs, const TArray<FName>& InBundles, const bool bAutoUnload)
{
	TArray<UElementusItemData*> Output;
	if (UAssetManager* const AssetManager = UAssetManager::GetIfValid())
	{
		Output = LoadElementusItemDatas_Internal(AssetManager, InIDs, InBundles, bAutoUnload);
	}
	return Output;
}

TArray<UElementusItemData*> UElementusInventoryFunctions::SearchItemData(const EElementusSearchType SearchType, const FString& SearchString, const TArray<FName>& InBundles, const bool bAutoUnload)
{
	TArray<UElementusItemData*> Output;
	if (UAssetManager* const AssetManager = UAssetManager::GetIfValid())
	{
		TArray<UElementusItemData*> ReturnedValues = LoadElementusItemDatas_Internal(AssetManager, TArray<FPrimaryElementusItemId>(), InBundles, bAutoUnload);

		for (UElementusItemData* const& Iterator : ReturnedValues)
		{
			bool bAddItem = false;
			switch (SearchType)
			{
			case EElementusSearchType::Name: bAddItem = Iterator->ItemName.ToString().Contains(SearchString);
				break;

			case EElementusSearchType::ID: bAddItem = FString::FromInt(Iterator->ItemId).Contains(SearchString);
				break;

			case EElementusSearchType::Type: bAddItem = static_cast<uint8>(Iterator->ItemType) == FCString::Atoi(*SearchString);
				break;

			default: break;
			}

			if (bAddItem)
			{
				UE_LOG(LogElementusInventory_Internal, Display, TEXT("Elementus Inventory - %s: Added Item Name: %s"), *FString(__func__), *Iterator->ItemName.ToString());

				Output.Add(Iterator);
			}
		}
	}

	return Output;
}

TArray<UElementusItemData*> UElementusInventoryFunctions::LoadElementusItemDatas_Internal(UAssetManager* InAssetManager, const TArray<FPrimaryElementusItemId>& InIDs, const TArray<FName>& InBundles, const bool bAutoUnload)
{
	TArray<UElementusItemData*> Output;
	const TArray<FPrimaryAssetId> PrimaryAssetIds(InIDs);

	const auto CheckAssetValidity_Lambda = [FuncName = __func__](UObject* const& InAsset) -> bool
	{
		const bool bOutput = IsValid(InAsset);
		if (IsValid(InAsset))
		{
			UE_LOG(LogElementusInventory_Internal, Display, TEXT("Elementus Inventory - %s: Item data %s found and loaded"), *FString(FuncName), *InAsset->GetName());
		}
		else
		{
			UE_LOG(LogElementusInventory_Internal, Error, TEXT("Elementus Inventory - %s: Failed to load item data: Invalid Asset"), *FString(FuncName));
		}

		return bOutput;
	};

	const auto PassItemArr_Lambda = [&CheckAssetValidity_Lambda, &Output](TArray<UObject*>& InArr)
	{
		if (InArr.IsEmpty())
		{
			UE_LOG(LogElementusInventory_Internal, Error, TEXT("Elementus Inventory - %s: Failed to find items with the given parameters"), *FString(__func__));
		}

		for (UObject* const& Iterator : InArr)
		{
			if (!CheckAssetValidity_Lambda(Iterator))
			{
				continue;
			}

			if (UElementusItemData* const CastedAsset = Cast<UElementusItemData>(Iterator))
			{
				Output.Add(CastedAsset);
			}
		}
	};

	if (const TSharedPtr<FStreamableHandle> StreamableHandle = InAssetManager->LoadPrimaryAssets(PrimaryAssetIds, InBundles);
		StreamableHandle.IsValid())
	{
		StreamableHandle->WaitUntilComplete(5.f);

		TArray<UObject*> LoadedAssets;
		StreamableHandle->GetLoadedAssets(LoadedAssets);
		PassItemArr_Lambda(LoadedAssets);
	}
	else // Objects already loaded
	{
		if (TArray<UObject*> LoadedAssets;
			InAssetManager->GetPrimaryAssetObjectList(FPrimaryAssetType(ElementusItemDataType), LoadedAssets))
		{
			PassItemArr_Lambda(LoadedAssets);
		}

		if (!Output.IsEmpty())
		{
			for (int32 Iterator = 0; Iterator < InIDs.Num(); ++Iterator)
			{
				if (!InIDs.Contains(Output[Iterator]->GetPrimaryAssetId()))
				{
					Output.RemoveAt(Iterator);
					--Iterator;
				}
			}
		}
	}

	if (bAutoUnload)
	{
		// Unload all elementus item assets
		InAssetManager->UnloadPrimaryAssets(PrimaryAssetIds);
	}
	return Output;
}

TArray<FPrimaryElementusItemId> UElementusInventoryFunctions::GetAllElementusItemIds()
{
	if (const UAssetManager* const AssetManager = UAssetManager::GetIfValid())
	{
		if (TArray<FPrimaryAssetId> IdList;
			AssetManager->GetPrimaryAssetIdList(FPrimaryAssetType(ElementusItemDataType), IdList))
		{
			return TArray<FPrimaryElementusItemId>(IdList);
		}
	}
	return TArray<FPrimaryElementusItemId>();
}

void UElementusInventoryFunctions::TradeElementusItem(TArray<FElementusItemInfo> ItemsToTrade, UElementusInventoryComponent* FromInventory, UElementusInventoryComponent* ToInventory)
{
	ItemsToTrade.RemoveAll([&FromInventory, &ToInventory](const FElementusItemInfo& InInfo)
	{
		return !FromInventory->CanGiveItem(InInfo) || !ToInventory->CanReceiveItem(InInfo);
	});

	if (ItemsToTrade.IsEmpty())
	{
		return;
	}

	FromInventory->UpdateElementusItems(ItemsToTrade, EElementusInventoryUpdateOperation::Remove);
	ToInventory->UpdateElementusItems(ItemsToTrade, EElementusInventoryUpdateOperation::Add);
}

bool UElementusInventoryFunctions::IsItemValid(const FElementusItemInfo InItemInfo)
{
	return InItemInfo.ItemId.IsValid();
}

bool UElementusInventoryFunctions::IsItemStackable(const FElementusItemInfo InItemInfo)
{
	if (const UElementusItemData* const ItemData = GetSingleItemDataById(InItemInfo.ItemId, {"Data"}))
	{
		return ItemData->bIsStackable;
	}

	return true;
}

FGameplayTagContainer UElementusInventoryFunctions::GetItemTagsWithParentTag(const FElementusItemInfo InItemInfo, const FGameplayTag FromParentTag)
{
	FGameplayTagContainer Output;
	for (const FGameplayTag& Iterator : InItemInfo.Tags)
	{
		if (Iterator.MatchesTag(FromParentTag))
		{
			Output.AddTag(Iterator);
		}
	}

	return Output;
}
