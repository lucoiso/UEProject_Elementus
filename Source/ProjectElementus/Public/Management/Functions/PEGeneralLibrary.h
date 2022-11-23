// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PEGeneralLibrary.generated.h"

/**
 * 
 */
UCLASS(Category = "Project Elementus | Classes")
class PROJECTELEMENTUS_API UPEGeneralLibrary final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Converts the passed doubles into CurrentValue/MaxValue formatted text */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "To Current/Max Text (Float)", AdvancedDisplay = "1", BlueprintAutocast), Category = "Utilities|Text")
	static FText Conv_DoubleToText_CurrentMax(double CurrentValue, double MaxValue, TEnumAsByte<ERoundingMode> RoundingMode, bool bAlwaysSign = false, bool bUseGrouping = true, int32 MinimumIntegralDigits = 1, int32 MaximumIntegralDigits = 324, int32 MinimumFractionalDigits = 0, int32 MaximumFractionalDigits = 3);
};
