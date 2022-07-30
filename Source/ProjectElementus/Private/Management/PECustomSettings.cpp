// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Management/PECustomSettings.h"

UPECustomSettings::UPECustomSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	UPECustomSettings::SetToDefaults();
}

void UPECustomSettings::ApplyCustomSettings(const bool bCheckForCommandLineOverrides) const
{
	const EConsoleVariableFlags Flags = bCheckForCommandLineOverrides ? ECVF_SetByCommandline : ECVF_SetByCode;

	if (IConsoleVariable* AntiAliasingCVar =
		IConsoleManager::Get().FindConsoleVariable(TEXT("r.DefaultFeature.AntiAliasing")))
	{
		AntiAliasingCVar->Set(AntiAliasingMode, Flags);
	}

	if (IConsoleVariable* FSREnabledCVar =
		IConsoleManager::Get().FindConsoleVariable(TEXT("r.FidelityFX.FSR.Enabled")))
	{
		FSREnabledCVar->Set(bFSREnabled, Flags);
	}

	if (IConsoleVariable* ScreenPercentageCVar =
			IConsoleManager::Get().FindConsoleVariable(TEXT("r.ScreenPercentage"));
		ScreenPercentageCVar && bFSREnabled)
	{
		switch (FSRMode)
		{
		case 0: // Performance
			ScreenPercentageCVar->Set(50, Flags);
			break;
		case 1: // Balanced
			ScreenPercentageCVar->Set(59, Flags);
			break;
		case 2: // Quality
			ScreenPercentageCVar->Set(67, Flags);
			break;
		case 3: // Ultra Quality
			ScreenPercentageCVar->Set(77, Flags);
			break;
		default: break;
		}
	}

	if (IConsoleVariable* TemporalUpsamplingCVar =
			IConsoleManager::Get().FindConsoleVariable(TEXT("r.TemporalAA.Upsampling"));
		TemporalUpsamplingCVar && AntiAliasingMode == 2)
	{
		TemporalUpsamplingCVar->Set(bEnableTemporalUpscaling, Flags);
	}
}

void UPECustomSettings::ApplySettings(const bool bCheckForCommandLineOverrides)
{
	Super::ApplySettings(bCheckForCommandLineOverrides);
	ApplyCustomSettings(bCheckForCommandLineOverrides);
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
