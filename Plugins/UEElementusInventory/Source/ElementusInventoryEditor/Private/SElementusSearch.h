// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEElementusInventory

#pragma once

#include "CoreMinimal.h"

enum class EElementusItemType : uint8;

DECLARE_DELEGATE_TwoParams(FOnElementusItemCheckStateChanged, ECheckBoxState, int32 ItemType);

class SElementusSearch final : public SCompoundWidget
{
public:
	SLATE_USER_ARGS(SElementusSearch)
	{
	}

	SLATE_EVENT(FOnElementusItemCheckStateChanged, OnCheckboxStateChanged)
	SLATE_EVENT
	(FOnTextChanged
	,
	OnSearchTextChanged
	)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

protected:
	void TriggerOnCheckboxStateChanged(ECheckBoxState NewState, int32 InType) const;
	void TriggerOnSearchTextChanged(const FText& InText) const;

private:
	FOnElementusItemCheckStateChanged OnCheckStateChanged;
	FOnTextChanged OnTextChangedDelegate;
};
