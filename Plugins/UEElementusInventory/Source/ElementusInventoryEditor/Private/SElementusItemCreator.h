// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEElementusInventory

#pragma once

#include "CoreMinimal.h"

class SElementusItemCreator final : public SCompoundWidget
{
public:
	SLATE_USER_ARGS(SElementusItemCreator)
	{
	}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

protected:
	FString GetObjPath(const int32 ObjId) const;
	void OnObjChanged(const FAssetData& AssetData, const int32 ObjId);

	const UClass* GetSelectedEntryClass() const;
	void HandleNewEntryClassSelected(const UClass* Class);

	void UpdateFolders();

	FReply HandleCreateItemButtonClicked() const;
	bool IsCreateEnabled() const;

private:
	TMap<int32, TWeakObjectPtr<UObject>> ObjectMap;
	TSharedPtr<FAssetThumbnailPool> ImageIcon_ThumbnailPool;
	TArray<TSharedPtr<FString>> ItemTypesArr;
	TArray<TSharedPtr<FString>> AssetFoldersArr;

	FName AssetName;
	FName AssetFolder;
	int32 ItemId = 1;
	TWeakObjectPtr<const UClass> ItemClass;
	FName ItemName;
	FText ItemDescription;
	uint8 ItemType = 0;
	bool bIsStackable = false;
	float ItemValue = 0.f;
	float ItemWeight = 0.f;
};
