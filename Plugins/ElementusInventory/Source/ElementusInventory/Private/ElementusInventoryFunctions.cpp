// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "ElementusInventoryFunctions.h"

bool UElementusInventoryFunctions::GetItemInfoByData(const UInventoryItemData* InData, TArray<FElementusItemInfo> InArr,
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

bool UElementusInventoryFunctions::GetItemInfoById(const int32 InId, TArray<FElementusItemInfo> InArr, int& ItemIndex)
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

UInventoryItemData* UElementusInventoryFunctions::GetUniqueElementusItemById(const FString SearchString,
                                                                             const UDataTable* InDataTable)
{
	if (const FElementusItemRowData* RowData =
			InDataTable->FindRow<FElementusItemRowData>(*SearchString, TEXT(""));
		RowData && !RowData->Data.IsNull())
	{
		return RowData->Data.LoadSynchronous();
	}

	return nullptr;
}

TArray<UInventoryItemData*> UElementusInventoryFunctions::SearchElementusItem(
	const EElementusSearchType SearchType, const FString SearchString, const UDataTable* InDataTable)
{
	TArray<UInventoryItemData*> Output;

	const auto ForEachRow_Lambda =
		[&Output, SearchType, SearchString]
	([[maybe_unused]] const FName& Key, const FElementusItemRowData& RowData)
	{
		if (!RowData.Data.IsNull())
		{
			UInventoryItemData* LoadedItem = RowData.Data.
			                                         LoadSynchronous();

			UE_LOG(LogTemp, Display,
			       TEXT("Elementus Inventory - SearchElementusItem: Returned Item Name: %s"),
			       *LoadedItem->ItemName.ToString());

			bool bAddItem = false;
			switch (SearchType)
			{
			case EElementusSearchType::Name:
				bAddItem = LoadedItem->ItemName.ToString().Contains(SearchString);
				break;

			case EElementusSearchType::ID:
				bAddItem = FString::FromInt(LoadedItem->ItemId).Contains(SearchString);
				break;

			case EElementusSearchType::Type:
				bAddItem = static_cast<int8>(LoadedItem->ItemType) == FCString::Atoi(*SearchString);
				break;

			default:
				break;
			}

			if (bAddItem)
			{
				Output.Add(LoadedItem);
			}
		}
	};

	InDataTable->ForeachRow<FElementusItemRowData>(TEXT(""), ForEachRow_Lambda);
	return Output;
}
