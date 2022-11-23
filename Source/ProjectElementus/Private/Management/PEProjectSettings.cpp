// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Management/PEProjectSettings.h"

UPEProjectSettings::UPEProjectSettings(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), PlayerColor(FLinearColor::Blue), BotColor(FLinearColor::Red), GravityMultiplier(1.f), SpeedMultiplier(1.f), JumpMultiplier(1.f), AirControlMultiplier(1.f)
{
	CategoryName = TEXT("Game");
}
