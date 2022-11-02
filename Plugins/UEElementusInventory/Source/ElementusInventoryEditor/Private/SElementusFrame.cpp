// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEElementusInventory

#include "SElementusFrame.h"
#include "SElementusSearch.h"
#include "SElementusTable.h"
#include "SElementusUtils.h"

void SElementusFrame::Construct([[maybe_unused]] const FArguments& InArgs)
{
	const TSharedRef<SElementusTable> Table = SNew(SElementusTable);

	ChildSlot
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		  .AutoWidth()
		  .MaxWidth(300.f)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SElementusSearch)
				.OnSearchTextChanged(Table, &SElementusTable::OnSearchTextModified)
				.OnCheckboxStateChanged(Table, &SElementusTable::OnSearchTypeModified)
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SElementusUtils)
				.TableSource(&Table.Get())
			]
		]
		+ SHorizontalBox::Slot()
		.FillWidth(1.f)
		[
			Table
		]
	];
}
