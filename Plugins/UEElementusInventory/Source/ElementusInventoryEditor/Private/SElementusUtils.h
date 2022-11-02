// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEElementusInventory

#pragma once

#include "CoreMinimal.h"

class SElementusTable;

class SElementusUtils final : public SCompoundWidget
{
public:
	SLATE_USER_ARGS(SElementusUtils) : _TableSource()
	{
	}

	SLATE_ARGUMENT(SElementusTable*, TableSource)
	SLATE_END_ARGS
	(
	)

	void Construct(const FArguments& InArgs);

protected:
	FReply OnButtonClicked(const uint32 ButtonId) const;

private:
	SElementusTable* TableSource = nullptr;
};
