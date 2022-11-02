// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEElementusInventory

#include "SElementusDetailsPanel.h"
#include "ElementusInventoryData.h"
#include "Engine/AssetManager.h"

void SElementusDetailsPanel::CustomizeHeader(const TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	PropertyHandlePtr = PropertyHandle;

	HeaderRow
		.NameContent()
		[
			PropertyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		[
			SNew(SObjectPropertyEntryBox)
			.AllowedClass(UElementusItemData::StaticClass())
			.PropertyHandle(PropertyHandle)
			.DisplayThumbnail(true)
			.ThumbnailPool(CustomizationUtils.GetThumbnailPool())
			.ObjectPath(this, &SElementusDetailsPanel::GetObjPath)
			.OnObjectChanged(this, &SElementusDetailsPanel::OnObjChanged)
			.OnShouldFilterAsset_Lambda([](const FAssetData& AssetData) -> bool
			{
				return AssetData.GetPrimaryAssetId().PrimaryAssetType != FPrimaryAssetType(ElementusItemDataType);
			})
		];
}

void SElementusDetailsPanel::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
}

void SElementusDetailsPanel::OnObjChanged(const FAssetData& AssetData) const
{
	// (PrimaryAssetType="VALUE",PrimaryAssetName="VALUE")
	const FString InValue(FString::Printf(TEXT("(PrimaryAssetType=\"%s\",PrimaryAssetName=\"%s\")"), *AssetData.GetPrimaryAssetId().PrimaryAssetType.ToString(), *AssetData.GetPrimaryAssetId().PrimaryAssetName.ToString()));

	ensure(PropertyHandlePtr->SetValueFromFormattedString(InValue) == FPropertyAccess::Result::Success);
}

FString SElementusDetailsPanel::GetObjPath() const
{
	if (const UAssetManager* const AssetManager = UAssetManager::GetIfValid();
		AssetManager && PropertyHandlePtr.IsValid())
	{
		FString AssetTypeValueStr;
		PropertyHandlePtr->GetChildHandle(GET_MEMBER_NAME_CHECKED(FPrimaryAssetId, PrimaryAssetType))->GetValueAsDisplayString(AssetTypeValueStr);

		FString AssetIdValueStr;
		PropertyHandlePtr->GetChildHandle(GET_MEMBER_NAME_CHECKED(FPrimaryAssetId, PrimaryAssetName))->GetValueAsDisplayString(AssetIdValueStr);

		const FPrimaryAssetId AssetId(*AssetTypeValueStr, *AssetIdValueStr);
		const FString Output = AssetId.IsValid() ? AssetManager->GetPrimaryAssetPath(AssetId).ToString() : FString();

		if (AssetId.IsValid() && Output.IsEmpty())
		{
			FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("Asset Manager could not retrieve asset information: Check if you've added the path to the Asset Manager settings.")));

			PropertyHandlePtr->ResetToDefault();
		}

		return Output;
	}

	return FString();
}
