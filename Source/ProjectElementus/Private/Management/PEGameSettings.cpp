// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Management/PEGameSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PEGameSettings)

UPEGameSettings::UPEGameSettings(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	UPEGameSettings::SetToDefaults();
}

void UPEGameSettings::ApplyPECustomSettings()
{
	if (IConsoleVariable* const AntiAliasingCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.AntiAliasingMethod")))
	{
		AntiAliasingCVar->Set(AntiAliasingMode, ECVF_SetByConsole);
	}

	if (IConsoleVariable* const FSREnabledCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.FidelityFX.FSR.Enabled")))
	{
		FSREnabledCVar->Set(bFSREnabled, ECVF_SetByConsole);
	}

	if (IConsoleVariable* const ScreenPercentageCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.ScreenPercentage"));
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

	if (IConsoleVariable* const TemporalUpsamplingCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.TemporalAA.Upsampling"));
		TemporalUpsamplingCVar && AntiAliasingMode == 2)
	{
		TemporalUpsamplingCVar->Set(bEnableTemporalUpscaling, ECVF_SetByConsole);
	}

	if (IConsoleVariable* const DynamicGICvar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.DynamicGlobalIlluminationMethod")))
	{
		// 0 = None | 1 = Lumen
		DynamicGICvar->Set(bEnableLumen, ECVF_SetByConsole);
	}

	if (IConsoleVariable* const DynamicGRCvar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.ReflectionMethod")))
	{
		// 1 = Lumen | 2 = Screen Space
		DynamicGRCvar->Set(bEnableLumen ? 1 : 2, ECVF_SetByConsole);
	}

	if (IConsoleVariable* const VirtualShadowCvar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Shadow.Virtual.Enable")))
	{
		// Enable Virtual Shadow Map if Lumen is enabled
		VirtualShadowCvar->Set(bEnableLumen, ECVF_SetByConsole);
	}
}

void UPEGameSettings::ApplyNonResolutionSettings()
{
	Super::ApplyNonResolutionSettings();
	ApplyPECustomSettings();
}

void UPEGameSettings::SetToDefaults()
{
	AntiAliasingMode = 1;
	bFSREnabled = true;
	FSRMode = 3;
	bEnableTemporalUpscaling = true;
	bEnableLumen = false;

	Super::SetToDefaults();

	ApplyPECustomSettings();
}

void UPEGameSettings::SetAntiAliasingMode(const int InMode)
{
	AntiAliasingMode = InMode;
}

int UPEGameSettings::GetAntiAliasingMode() const
{
	return AntiAliasingMode;
}

void UPEGameSettings::SetFSREnabled(const bool bEnable)
{
	bFSREnabled = bEnable;
}

bool UPEGameSettings::GetFSREnabled() const
{
	return bFSREnabled;
}

void UPEGameSettings::SetFSRMode(const int InMode)
{
	FSRMode = InMode;
}

int UPEGameSettings::GetFSRMode() const
{
	return FSRMode;
}

void UPEGameSettings::SetTemporalUpscalingEnabled(const bool bEnable)
{
	bEnableTemporalUpscaling = bEnable;
}

bool UPEGameSettings::GetTemporalUpscalingEnabled() const
{
	return bEnableTemporalUpscaling;
}

void UPEGameSettings::SetLumenEnabled(const bool bEnable)
{
	bEnableLumen = bEnable;
}

bool UPEGameSettings::GetLumenEnabled() const
{
	return bEnableLumen;
}

UPEGameSettings* UPEGameSettings::GetCustomGameUserSettings()
{
	return Cast<UPEGameSettings>(GetGameUserSettings());
}
