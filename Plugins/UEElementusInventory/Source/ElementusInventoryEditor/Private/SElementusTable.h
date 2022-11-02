// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEElementusInventory

#pragma once

#include "CoreMinimal.h"
#include "ElementusInventoryFunctions.h"

struct FElementusItemRowData
{
	explicit FElementusItemRowData(const FPrimaryElementusItemId& InPrimaryAssetId)
	{
		const auto ItemData = UElementusInventoryFunctions::GetSingleItemDataById(InPrimaryAssetId, {TEXT("Data"), TEXT("SoftData"),}, false);

		PrimaryAssetId = InPrimaryAssetId;
		Id = ItemData->ItemId;
		Name = ItemData->ItemName;
		Type = ItemData->ItemType;
		Class = ItemData->ItemClass.IsValid() ? *ItemData->ItemClass.LoadSynchronous()->GetName() : FName();
		Object = ItemData->ItemObject.IsValid() ? *ItemData->ItemObject.LoadSynchronous()->GetName() : FName();
		Value = ItemData->ItemValue;
		Weight = ItemData->ItemWeight;

		UElementusInventoryFunctions::UnloadElementusItem(InPrimaryAssetId);
	}

	FPrimaryAssetId PrimaryAssetId;
	int32 Id = -1;
	FName Name = NAME_None;
	EElementusItemType Type = EElementusItemType::None;
	FName Class = NAME_None;
	FName Object = NAME_None;
	float Value = -1.f;
	float Weight = -1.f;
};

using FElementusItemPtr = TSharedPtr<FElementusItemRowData, ESPMode::ThreadSafe>;

class SElementusTable final : public SCompoundWidget
{
public:
	SLATE_USER_ARGS(SElementusTable)
	{
	}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	friend class SElementusFrame;
	friend class SElementusUtils;

protected:
	TSharedRef<ITableRow> OnGenerateWidgetForList(TSharedPtr<FElementusItemRowData> InItem, const TSharedRef<STableViewBase>& OwnerTable) const;
	void OnTableItemDoubleClicked(TSharedPtr<FElementusItemRowData> ElementusItemRowData) const;
	void OnColumnSort(EColumnSortPriority::Type SortPriority, const FName& ColumnName, EColumnSortMode::Type SortMode);
	EColumnSortMode::Type GetColumnSort(const FName ColumnId) const;
	EVisibility GetIsVisible(const FElementusItemPtr InItem) const;
	void OnSearchTextModified(const FText& InText);
	void OnSearchTypeModified(const ECheckBoxState InState, const int32 InType);
	void UpdateItemList();
	TArray<TSharedPtr<FElementusItemRowData>> GetSelectedItems() const;

private:
	TArray<TSharedPtr<FElementusItemRowData>> ItemArr;
	TArray<int32> AllowedTypes;
	FText SearchText;
	FName ColumnBeingSorted = NAME_None;
	EColumnSortMode::Type CurrentSortMode = EColumnSortMode::None;

	TSharedPtr<SListView<FElementusItemPtr>> EdListView;
};
