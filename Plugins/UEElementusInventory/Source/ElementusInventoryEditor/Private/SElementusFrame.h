// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEElementusInventory

#pragma once

#include "CoreMinimal.h"

class SElementusFrame final : public SCompoundWidget
{
public:
	SLATE_USER_ARGS(SElementusFrame)
	{
	}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
};
