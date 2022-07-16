// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "ElementusInventoryFunctions.h"
#include "ElementusInventoryComponent.h"
#include "Engine/AssetManager.h"

bool UElementusInventoryFunctions::CompareItemInfoIds(const FPrimaryAssetId& Info1, const FPrimaryAssetId& Info2)
{
	return Info1 == Info2;
}

bool UElementusInventoryFunctions::CompareItemDataIds(const UInventoryItemData* Data1, const UInventoryItemData* Data2)
{
	return Data1->GetPrimaryAssetId() == Data2->GetPrimaryAssetId();
}

UInventoryItemData* UElementusInventoryFunctions::GetElementusItemDataById(const FPrimaryAssetId& InID,
                                                                           const TArray<FName>& InBundles)
{
	UInventoryItemData* Output = nullptr;
	if (UAssetManager* AssetManager = UAssetManager::GetIfValid())
	{
		if (const TSharedPtr<FStreamableHandle> StreamableHandle =
				AssetManager->LoadPrimaryAsset(InID, InBundles);
			StreamableHandle.IsValid())
		{
			StreamableHandle->WaitUntilComplete(5.f);
			Output = Cast<UInventoryItemData>(StreamableHandle->GetLoadedAsset());
		}
		else // The object is already loaded
		{
			Output = AssetManager->GetPrimaryAssetObject<UInventoryItemData>(InID);
		}

		AssetManager->UnloadPrimaryAsset(InID);
	}

	return Output;
}

TArray<UInventoryItemData*> UElementusInventoryFunctions::GetElementusItemDataArrayById(
	const TArray<FPrimaryAssetId> InIDs,
	const TArray<FName>& InBundles)
{
	TArray<UInventoryItemData*> Output;
	if (UAssetManager* AssetManager = UAssetManager::GetIfValid())
	{
		Output = LoadElementusItemDatas_Internal(AssetManager, InIDs, InBundles);
	}
	return Output;
}

TArray<UInventoryItemData*> UElementusInventoryFunctions::SearchElementusItemData(const EElementusSearchType SearchType,
	const FString& SearchString, const TArray<FName>& InBundles)
{
	TArray<UInventoryItemData*> Output;
	if (UAssetManager* AssetManager = UAssetManager::GetIfValid())
	{
		TArray<UInventoryItemData*> ReturnedValues =
			LoadElementusItemDatas_Internal(AssetManager, TArray<FPrimaryAssetId>(), InBundles);

		for (const auto& Iterator : ReturnedValues)
		{
			bool bAddItem = false;
			switch (SearchType)
			{
			case EElementusSearchType::Name:
				bAddItem = Iterator->ItemName.ToString().Contains(SearchString);
				break;

			case EElementusSearchType::ID:
				bAddItem = FString::FromInt(Iterator->ItemId).Contains(SearchString);
				break;

			case EElementusSearchType::Type:
				bAddItem = static_cast<int8>(Iterator->ItemType) == FCString::Atoi(*SearchString);
				break;

			default:
				break;
			}

			if (bAddItem)
			{
				UE_LOG(LogElementusInventory_Internal, Display,
				       TEXT("Elementus Inventory - %s: Added Item Name: %s"),
				       *FString(__func__), *Iterator->ItemName.ToString());

				Output.Add(Iterator);
			}
		}
	}

	return Output;
}

TArray<UInventoryItemData*> UElementusInventoryFunctions::LoadElementusItemDatas_Internal(UAssetManager* InAssetManager,
	const TArray<FPrimaryAssetId> InIDs,
	const TArray<FName>& InBundles)
{
	TArray<UInventoryItemData*> Output;

	constexpr auto& FuncNam_LambVer = __func__;
	const auto& CheckAssetValidity_Lambda =
		[&FuncNam_LambVer](UObject* InAsset) -> bool
	{
		const bool bOutput = IsValid(InAsset);
		if (IsValid(InAsset))
		{
			UE_LOG(LogElementusInventory_Internal, Display,
			       TEXT("Elementus Inventory - %s: Item data %s found and loaded"),
			       *FString(FuncNam_LambVer), *InAsset->GetName());
		}
		else
		{
			UE_LOG(LogElementusInventory_Internal, Error,
			       TEXT("Elementus Inventory - %s: Failed to load item data: Invalid Asset"),
			       *FString(FuncNam_LambVer));
		}

		return bOutput;
	};

	const auto& PassItemArr_Lambda =
		[&CheckAssetValidity_Lambda, &Output](TArray<UObject*>& InArr)
	{
		if (InArr.IsEmpty())
		{
			UE_LOG(LogElementusInventory_Internal, Error,
			       TEXT("Elementus Inventory - %s: Failed to find items with the given parameters"),
			       *FString(__func__));
		}

		for (UObject* Iterator : InArr)
		{
			if (!CheckAssetValidity_Lambda(Iterator))
			{
				continue;
			}

			if (UInventoryItemData* CastedAsset = Cast<UInventoryItemData>(Iterator))
			{
				Output.Add(CastedAsset);
			}
		}
	};

	if (const TSharedPtr<FStreamableHandle> StreamableHandle =
			InAssetManager->LoadPrimaryAssets(InIDs, InBundles);
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

		for (int Iterator = 0; Iterator < InIDs.Num(); ++Iterator)
		{
			if (!InIDs.Contains(Output[Iterator]->GetPrimaryAssetId()))
			{
				Output.RemoveAt(Iterator);
				--Iterator;
			}
		}
	}

	// Unload all elementus item assets
	InAssetManager->UnloadPrimaryAssetsWithType(FPrimaryAssetType(ElementusItemDataType));
	return Output;
}

TArray<FPrimaryAssetId> UElementusInventoryFunctions::GetElementusItemIds()
{
	if (const UAssetManager* AssetManager = UAssetManager::GetIfValid())
	{
		if (TArray<FPrimaryAssetId> IdList;
			AssetManager->GetPrimaryAssetIdList(FPrimaryAssetType(ElementusItemDataType), IdList))
		{
			return IdList;
		}
	}
	return TArray<FPrimaryAssetId>();
}

void UElementusInventoryFunctions::TradeElementusItem(TMap<FPrimaryAssetId, int32> ItemsToTrade,
                                                      UElementusInventoryComponent* FromInventory,
                                                      UElementusInventoryComponent* ToInventory)
{
	for (auto& Iterator : ItemsToTrade)
	{
		Iterator.Value =
			FMath::Clamp(Iterator.Value, 0, FromInventory->GetItemStack().FindRef(Iterator.Key));

		FromInventory->DiscardElementusItem(Iterator.Key, Iterator.Value);
		ToInventory->AddElementusItem(Iterator.Key, Iterator.Value);
	}
}
