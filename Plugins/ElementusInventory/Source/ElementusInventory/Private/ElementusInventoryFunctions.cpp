// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "ElementusInventoryFunctions.h"
#include "Engine/AssetManager.h"

bool UElementusInventoryFunctions::FindElementusItemInfoByDataInArr(const UInventoryItemData* InData,
                                                                    TArray<FElementusItemInfo> InArr,
                                                                    int& ItemIndex)
{
	for (auto Iterator = InArr.CreateIterator(); Iterator; ++Iterator)
	{
		if (Iterator->ItemData == InData)
		{
			ItemIndex = Iterator.GetIndex();

			UE_LOG(LogTemp, Display,
			       TEXT("Elementus Inventory - %s: Found item with index: %d"), *FString(__func__), ItemIndex);

			return true;
		}
	}

	return false;
}

bool UElementusInventoryFunctions::FindElementusItemInfoByIdInArr(const int32 InId,
                                                                  TArray<FElementusItemInfo> InArr,
                                                                  int& ItemIndex)
{
	for (auto Iterator = InArr.CreateIterator(); Iterator; ++Iterator)
	{
		if (Iterator->ItemData->ItemId == InId)
		{
			ItemIndex = Iterator.GetIndex();

			UE_LOG(LogTemp, Display,
			       TEXT("Elementus Inventory - %s: Found item with index: %d"), *FString(__func__), ItemIndex);

			return true;
		}
	}

	return false;
}

UInventoryItemData* UElementusInventoryFunctions::GetElementusItemDataById(const FString InID)
{
	if (UAssetManager* AssetManager = UAssetManager::GetIfValid())
	{
		if (const TSharedPtr<FStreamableHandle> StreamableHandle =
				AssetManager->LoadPrimaryAsset(
					FPrimaryAssetId(TEXT("ElementusInventory_ItemData"), *FString("Item_" + InID)));
			StreamableHandle.IsValid())
		{
			StreamableHandle->WaitUntilComplete(2.5f);
			UObject* OutputAsset = StreamableHandle->GetLoadedAsset();

			if (IsValid(OutputAsset))
			{
				UE_LOG(LogTemp, Display,
				       TEXT("Elementus Inventory - %s: Found item data: %s"),
				       *FString(__func__), *OutputAsset->GetName());
			}
			else
			{
				UE_LOG(LogTemp, Error,
				       TEXT("Elementus Inventory - %s: Failed to load item data"),
				       *FString(__func__));
			}

			return Cast<UInventoryItemData>(OutputAsset);
		}
	}

	return nullptr;
}

TArray<UInventoryItemData*> UElementusInventoryFunctions::SearchElementusItemData(const EElementusSearchType SearchType,
	const FString SearchString)
{
	TArray<UInventoryItemData*> OutputArr;

	if (UAssetManager* AssetManager = UAssetManager::GetIfValid())
	{
		if (TArray<FPrimaryAssetId> IdList;
			AssetManager->GetPrimaryAssetIdList(TEXT("ElementusInventory_ItemData"), IdList))
		{
			if (const TSharedPtr<FStreamableHandle> StreamableHandle =
					AssetManager->LoadPrimaryAssets(IdList, TArray<FName>{TEXT("Data")});
				StreamableHandle.IsValid())
			{
				StreamableHandle->WaitUntilComplete(10.f);

				TArray<UObject*> LoadedAssets;
				StreamableHandle->GetLoadedAssets(LoadedAssets);

				if (LoadedAssets.IsEmpty())
				{
					UE_LOG(LogTemp, Error,
					       TEXT("Elementus Inventory - %s: Failed to load items data"),
					       *FString(__func__));
				}

				for (UObject* Iterator : LoadedAssets)
				{
					if (!IsValid(Iterator))
					{
						continue;
					}

					if (UInventoryItemData* CastedAsset = Cast<UInventoryItemData>(Iterator))
					{
						UE_LOG(LogTemp, Display,
						       TEXT("Elementus Inventory - %s: Returned Item Name: %s"),
						       *FString(__func__), *CastedAsset->ItemName.ToString());

						bool bAddItem = false;
						switch (SearchType)
						{
						case EElementusSearchType::Name:
							bAddItem = CastedAsset->ItemName.ToString().Contains(SearchString);
							break;

						case EElementusSearchType::ID:
							bAddItem = FString::FromInt(CastedAsset->ItemId).Contains(SearchString);
							break;

						case EElementusSearchType::Type:
							bAddItem = static_cast<int8>(CastedAsset->ItemType) == FCString::Atoi(*SearchString);
							break;

						default:
							break;
						}

						if (bAddItem)
						{
							UE_LOG(LogTemp, Display,
							       TEXT("Elementus Inventory - %s: Added Item Name: %s"),
							       *FString(__func__), *CastedAsset->ItemName.ToString());

							OutputArr.Add(CastedAsset);
						}
					}
				}
			}
		}
	}

	return OutputArr;
}
