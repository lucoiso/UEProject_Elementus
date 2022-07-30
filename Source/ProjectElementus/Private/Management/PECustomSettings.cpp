// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Management/PECustomSettings.h"

UPECustomSettings::UPECustomSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	UPECustomSettings::SetToDefaults();
}

void UPECustomSettings::ApplyNonResolutionSettings()
{
	Super::ApplyNonResolutionSettings();
	
	if (IConsoleVariable* AntiAliasingCVar =
		IConsoleManager::Get().FindConsoleVariable(TEXT("r.AntiAliasingMethod")))
	{
		AntiAliasingCVar->Set(AntiAliasingMode, ECVF_SetByConsole);
	}

	if (IConsoleVariable* FSREnabledCVar =
		IConsoleManager::Get().FindConsoleVariable(TEXT("r.FidelityFX.FSR.Enabled")))
	{
		FSREnabledCVar->Set(bFSREnabled, ECVF_SetByConsole);
	}

	if (IConsoleVariable* ScreenPercentageCVar =
			IConsoleManager::Get().FindConsoleVariable(TEXT("r.ScreenPercentage"));
		ScreenPercentageCVar && bFSREnabled)
	{
		switch (FSRMode)
		{
		case 0: // Performance
			ScreenPercentageCVar->Set(50, ECVF_SetByConsole);
			break;
		case 1: // Balanced
			ScreenPercentageCVar->Set(59, ECVF_SetByConsole);
			break;
		case 2: // Quality
			ScreenPercentageCVar->Set(67, ECVF_SetByConsole);
			break;
		case 3: // Ultra Quality
			ScreenPercentageCVar->Set(77, ECVF_SetByConsole);
			break;
		default: break;
		}
	}

	if (IConsoleVariable* TemporalUpsamplingCVar =
			IConsoleManager::Get().FindConsoleVariable(TEXT("r.TemporalAA.Upsampling"));
		TemporalUpsamplingCVar && AntiAliasingMode == 2)
	{
		TemporalUpsamplingCVar->Set(bEnableTemporalUpscaling, ECVF_SetByConsole);
	}
}

void UPECustomSettings::SetToDefaults()
{
	AntiAliasingMode = 3;
	bFSREnabled = true;
	FSRMode = 3;
	bEnableTemporalUpscaling = true;

	Super::SetToDefaults();
}

void UPECustomSettings::SetAntiAliasingMode(const int InMode)
{
	AntiAliasingMode = InMode;
}

int UPECustomSettings::GetAntiAliasingMode() const
{
	return AntiAliasingMode;
}

void UPECustomSettings::SetFSREnabled(const bool bEnable)
{
	bFSREnabled = bEnable;
}

bool UPECustomSettings::GetFSREnabled() const
{
	return bFSREnabled;
}

void UPECustomSettings::SetFSRMode(const int InMode)
{
	FSRMode = InMode;
}

int UPECustomSettings::GetFSRMode() const
{
	return FSRMode;
}

void UPECustomSettings::SetTemporalUpscalingEnabled(const bool bEnable)
{
	bEnableTemporalUpscaling = bEnable;
}

bool UPECustomSettings::GetTemporalUpscalingEnabled() const
{
	return bEnableTemporalUpscaling;
}

UPECustomSettings* UPECustomSettings::GetCustomGameUserSettings()
{
	return Cast<UPECustomSettings>(GetGameUserSettings());
}
