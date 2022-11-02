// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEElementusInventory

#include "SElementusItemCreator.h"
#include "PropertyCustomizationHelpers.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "AssetThumbnail.h"
#include "AssetToolsModule.h"
#include "ElementusInventoryData.h"
#include "ElementusInventoryEditorFunctions.h"
#include "Engine/AssetManager.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Widgets/Input/STextComboBox.h"
#include "PackageTools.h"
#include "Factories/DataAssetFactory.h"
#include "UObject/SavePackage.h"
#include "EditorStyleSet.h"

void SElementusItemCreator::Construct([[maybe_unused]] const FArguments&)
{
	constexpr float Slot_Padding = 1.f;

	ImageIcon_ThumbnailPool = MakeShareable(new FAssetThumbnailPool(1024));
	const ISlateStyle& AppStyle = FAppStyle::Get();

	const auto CenterTextCreator_Lambda = [&AppStyle](const FString& InStr) -> const TSharedRef<STextBlock>
	{
		return SNew(STextBlock)
			.Text(FText::FromString(InStr))
			.TextStyle(AppStyle, "PropertyEditor.AssetClass")
			.Font(AppStyle.GetFontStyle("PropertyWindow.NormalFont"))
			.Justification(ETextJustify::Left)
			.Margin(4.f);
	};

	const auto ObjEntryBoxCreator_Lambda = [this](UClass* ObjClass, const int32 ObjId) -> const TSharedRef<SObjectPropertyEntryBox>
	{
		return SNew(SObjectPropertyEntryBox)
			.IsEnabled(true)
			.AllowedClass(ObjClass)
			.AllowClear(true)
			.DisplayUseSelected(true)
			.DisplayBrowse(true)
			.DisplayThumbnail(true)
			.ThumbnailPool(ImageIcon_ThumbnailPool.ToSharedRef())
			.ObjectPath(this, &SElementusItemCreator::GetObjPath, ObjId)
			.OnObjectChanged(this, &SElementusItemCreator::OnObjChanged, ObjId);
	};

	const auto ContentPairCreator_Lambda = [this, &AppStyle](const TSharedRef<SWidget> Content1, const TSharedRef<SWidget> Content2) -> const TSharedRef<SBorder>
	{
		return SNew(SBorder)
			.BorderImage(AppStyle.GetBrush("ToolPanel.GroupBorder"))
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				  .FillWidth(0.5f)
				  .HAlign(HAlign_Fill)
				  .VAlign(VAlign_Center)
				[
					Content1
				]
				+ SHorizontalBox::Slot()
				  .FillWidth(0.5f)
				  .MaxWidth(250.f)
				  .HAlign(HAlign_Fill)
				  .VAlign(VAlign_Center)
				[
					Content2
				]
			];
	};

	ItemTypesArr = ElementusEdHelper::GetEnumValuesAsStringArray();
	UpdateFolders();

	const TSharedRef<SToolTip> ToolTip = SNew(SToolTip)
		.Text(FText::FromString(TEXT("Already exists a item with this Id.")))
		.Visibility_Lambda([this]() -> EVisibility
	    {
		    return IsCreateEnabled() ? EVisibility::Collapsed : EVisibility::Visible;
	    });

	ChildSlot
	[
		SNew(SBorder)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			  .Padding(Slot_Padding)
			  .AutoHeight()
			[
				ContentPairCreator_Lambda(CenterTextCreator_Lambda("Item Id"), SNew(SNumericEntryBox<int32>)
					.AllowSpin(false)
					.MinValue(1)
					.Value_Lambda([this] { return ItemId; })
					.OnValueChanged_Lambda([this](const int32 InValue)
					{
						ItemId = InValue;
					}))
			]
			+ SVerticalBox::Slot()
			  .Padding(Slot_Padding)
			  .AutoHeight()
			[
				ContentPairCreator_Lambda(CenterTextCreator_Lambda("Item Object"), ObjEntryBoxCreator_Lambda(UObject::StaticClass(), 0))
			]
			+ SVerticalBox::Slot()
			  .Padding(Slot_Padding)
			  .AutoHeight()
			[
				ContentPairCreator_Lambda(CenterTextCreator_Lambda("Item Class"), SNew(SClassPropertyEntryBox)
					.AllowAbstract(true)
					.SelectedClass(this, &SElementusItemCreator::GetSelectedEntryClass)
					.OnSetClass(this, &SElementusItemCreator::HandleNewEntryClassSelected))
			]
			+ SVerticalBox::Slot()
			  .Padding(Slot_Padding)
			  .AutoHeight()
			[
				ContentPairCreator_Lambda(CenterTextCreator_Lambda("Item Name"), SNew(SEditableTextBox)
					.OnTextChanged(FOnTextChanged::CreateLambda([this](const FText& InText)
					{
					    ItemName = *InText.ToString();
					})))
			]
			+ SVerticalBox::Slot()
			  .Padding(Slot_Padding)
			  .AutoHeight()
			[
				ContentPairCreator_Lambda(CenterTextCreator_Lambda("Item Description"), SNew(SMultiLineEditableTextBox)
					.OnTextChanged(FOnTextChanged::CreateLambda([this](const FText& InText)
					{
					    ItemDescription = InText;
					})))
			]
			+ SVerticalBox::Slot()
			  .Padding(Slot_Padding)
			  .AutoHeight()
			[
				ContentPairCreator_Lambda(CenterTextCreator_Lambda("Item Type"), SNew(STextComboBox)
					.OptionsSource(&ItemTypesArr)
					.OnSelectionChanged(STextComboBox::FOnTextSelectionChanged::CreateLambda([this](const TSharedPtr<FString>& InStr, [[maybe_unused]] ESelectInfo::Type)
				    {
					    ItemType = ItemTypesArr.Find(InStr);
				    })))
			]
			+ SVerticalBox::Slot()
			  .Padding(Slot_Padding)
			  .AutoHeight()
			[
				ContentPairCreator_Lambda(CenterTextCreator_Lambda("Is Stackable"), SNew(SCheckBox)
					.IsChecked(ECheckBoxState::Checked)
					.OnCheckStateChanged_Lambda([this](const ECheckBoxState InState)
				    {
					    bIsStackable = InState == ECheckBoxState::Checked;
				    }))
			]
			+ SVerticalBox::Slot()
			  .Padding(Slot_Padding)
			  .AutoHeight()
			[
				ContentPairCreator_Lambda(CenterTextCreator_Lambda("Item Value"), SNew(SNumericEntryBox<float>)
					.AllowSpin(false)
					.MinValue(0.0f)
					.Value_Lambda([this] { return ItemValue; })
					.OnValueChanged_Lambda([this](const float InValue)
				    {
					    ItemValue = InValue;
				    }))
			]
			+ SVerticalBox::Slot()
			  .Padding(Slot_Padding)
			  .AutoHeight()
			[
				ContentPairCreator_Lambda(CenterTextCreator_Lambda("Item Weight"), SNew(SNumericEntryBox<float>)
					.AllowSpin(false)
					.MinValue(0.0f)
					.Value_Lambda([this] { return ItemWeight; })
					.OnValueChanged_Lambda([this](const float InValue)
				    {
					    ItemWeight = InValue;
				    }))
			]
			+ SVerticalBox::Slot()
			  .Padding(Slot_Padding)
			  .AutoHeight()
			[
				ContentPairCreator_Lambda(CenterTextCreator_Lambda("Item Icon"), ObjEntryBoxCreator_Lambda(UTexture2D::StaticClass(), 1))
			]
			+ SVerticalBox::Slot()
			  .Padding(Slot_Padding)
			  .AutoHeight()
			[
				ContentPairCreator_Lambda(CenterTextCreator_Lambda("Item Image"), ObjEntryBoxCreator_Lambda(UTexture2D::StaticClass(), 2))
			]
			+ SVerticalBox::Slot()
			  .Padding(Slot_Padding)
			  .AutoHeight()
			[
				ContentPairCreator_Lambda(CenterTextCreator_Lambda("Asset Name"), SNew(SEditableTextBox)
					.OnTextChanged(FOnTextChanged::CreateLambda([this](const FText& InText)
					{
					    AssetName = *InText.ToString();
					})))
			]
			+ SVerticalBox::Slot()
			  .Padding(Slot_Padding)
			  .AutoHeight()
			[
				ContentPairCreator_Lambda(CenterTextCreator_Lambda("Asset Folder"), SNew(SHorizontalBox)
				    + SHorizontalBox::Slot()
				    [
					    SNew(STextComboBox)
						.OptionsSource(&AssetFoldersArr)
						.OnSelectionChanged(STextComboBox::FOnTextSelectionChanged::CreateLambda(
						[this](const TSharedPtr<FString>& InStr, [[maybe_unused]] ESelectInfo::Type)
						{
						    AssetFolder = FName(*InStr.Get());
						}))
				    ]
				    + SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SButton)
						.OnClicked(FOnClicked::CreateLambda([this]() -> FReply
						{
						    UpdateFolders();
						    return FReply::Handled();
						}))
						.Content()
						[
						    SNew(SImage)
						    .Image(AppStyle.GetBrush("Icons.Refresh"))
						]
					])
			]
			+ SVerticalBox::Slot()
			  .Padding(Slot_Padding * 2.f)
			  .HAlign(HAlign_Center)
			  .AutoHeight()
			[
				SNew(SButton)
				.Text(FText::FromString("Create Item"))
				.OnClicked(this, &SElementusItemCreator::HandleCreateItemButtonClicked)
				.IsEnabled(this, &SElementusItemCreator::IsCreateEnabled)
				.ToolTip(ToolTip)
			]
		]
	];
}

void SElementusItemCreator::OnObjChanged(const FAssetData& AssetData, const int32 ObjId)
{
	ObjectMap.FindOrAdd(ObjId) = AssetData.GetAsset();
}

FString SElementusItemCreator::GetObjPath(const int32 ObjId) const
{
	return ObjectMap.Contains(ObjId) && ObjectMap.FindRef(ObjId).IsValid() ? ObjectMap.FindRef(ObjId)->GetPathName() : FString();
}

void SElementusItemCreator::HandleNewEntryClassSelected(const UClass* Class)
{
	ItemClass = Class;
}

const UClass* SElementusItemCreator::GetSelectedEntryClass() const
{
	return ItemClass.Get();
}

void SElementusItemCreator::UpdateFolders()
{
	AssetFoldersArr.Empty();

	if (const UAssetManager* const AssetManager = UAssetManager::GetIfValid())
	{
		if (FPrimaryAssetTypeInfo Info;
			AssetManager->GetPrimaryAssetTypeInfo(FPrimaryAssetType(ElementusItemDataType), Info))
		{
			for (const FString& Path : Info.AssetScanPaths)
			{
				AssetFoldersArr.Add(MakeShareable(new FString(Path)));
			}
		}
	}

	if (const UAssetManager* const AssetManager = UAssetManager::GetIfValid();
		IsValid(AssetManager) && AssetManager->HasInitialScanCompleted() && AssetFoldersArr.IsEmpty())
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("Asset Manager could not find any folder. Please check your Asset Manager settings.")));
	}
}

FReply SElementusItemCreator::HandleCreateItemButtonClicked() const
{
	if (AssetFolder.IsNone() || AssetName.IsNone())
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("Please enter the asset name and folder for the new item."));

		return FReply::Handled();
	}

	const FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools");

	const FString PackageName = UPackageTools::SanitizePackageName(AssetFolder.ToString() + TEXT("/") + AssetName.ToString());

	UDataAssetFactory* const Factory = NewObject<UDataAssetFactory>();

	if (UObject* const NewData = AssetToolsModule.Get().CreateAsset(AssetName.ToString(), FPackageName::GetLongPackagePath(PackageName), UElementusItemData::StaticClass(), Factory);
		NewData != nullptr)
	{
		UElementusItemData* const ItemData = Cast<UElementusItemData>(NewData);
		ItemData->ItemId = ItemId;
		ItemData->ItemObject = TSoftObjectPtr(Cast<UObject>(ObjectMap.FindRef(0)));
		ItemData->ItemClass = TSoftClassPtr(ItemClass.Get());
		ItemData->ItemName = ItemName;
		ItemData->ItemDescription = ItemDescription;
		ItemData->ItemType = static_cast<EElementusItemType>(ItemType);
		ItemData->bIsStackable = bIsStackable;
		ItemData->ItemValue = ItemValue;
		ItemData->ItemWeight = ItemWeight;
		ItemData->ItemIcon = Cast<UTexture2D>(ObjectMap.FindRef(1));
		ItemData->ItemImage = Cast<UTexture2D>(ObjectMap.FindRef(2));

		TArray<FAssetData> SyncAssets;
		SyncAssets.Add(FAssetData(ItemData));
		GEditor->SyncBrowserToObjects(SyncAssets);

		const FSavePackageArgs SaveArgs;
		const FString TempPackageName = ItemData->GetPackage()->GetName();
		const FString TempPackageFilename = FPackageName::LongPackageNameToFilename(TempPackageName, FPackageName::GetAssetPackageExtension());

		GEditor->Save(ItemData->GetPackage(), ItemData, *TempPackageFilename, SaveArgs);
	}

	return FReply::Handled();
}

bool SElementusItemCreator::IsCreateEnabled() const
{
	if (const UAssetManager* const AssetManager = UAssetManager::GetIfValid())
	{
		const FPrimaryAssetId InId(ElementusItemDataType, *("Item_" + FString::FromInt(ItemId)));

		return ItemId != 0 && !AssetManager->GetPrimaryAssetPath(InId).IsValid();
	}

	return false;
}
