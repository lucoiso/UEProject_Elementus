// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Management/Functions/PEGeneralLibrary.h"
#include "Kismet/KismetTextLibrary.h"

#define LOCTEXT_NAMESPACE "ProjectElementus_GeneralLibrary"

FText UPEGeneralLibrary::Conv_DoubleToText_CurrentMax(double CurrentValue, double MaxValue, TEnumAsByte<ERoundingMode> RoundingMode, bool bAlwaysSign, bool bUseGrouping, int32 MinimumIntegralDigits, int32 MaximumIntegralDigits, int32 MinimumFractionalDigits, int32 MaximumFractionalDigits)
{
	const FText CurrentValueText = UKismetTextLibrary::Conv_DoubleToText(CurrentValue, RoundingMode, bAlwaysSign, bUseGrouping, MinimumIntegralDigits, MaximumIntegralDigits, MinimumFractionalDigits, MaximumFractionalDigits);

	const FText MaxValueText = UKismetTextLibrary::Conv_DoubleToText(MaxValue, RoundingMode, bAlwaysSign, bUseGrouping, MinimumIntegralDigits, MaximumIntegralDigits, MinimumFractionalDigits, MaximumFractionalDigits);

	return FText::Format(LOCTEXT("DoubleToText_CurrentMax", "{0}/{1}"), CurrentValueText, MaxValueText);
}

#undef LOCTEXT_NAMESPACE