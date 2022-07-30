// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "PECustomSettings.generated.h"

/**
 * 
 */
UCLASS(Config=GameUserSettings, configdonotcheckdefaults, Category = "Project Elementus | Classes")
class PROJECTELEMENTUS_API UPECustomSettings final : public UGameUserSettings
{
	GENERATED_BODY()

public:
	explicit UPECustomSettings(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = Settings)
	void ApplyCustomSettings(const bool bCheckForCommandLineOverrides = false) const;

	// Start of UGameUserSettings	
	virtual void ApplySettings(bool bCheckForCommandLineOverrides) override;
	virtual void SetToDefaults() override;
	// End of UGameUserSettings

	UFUNCTION(BlueprintCallable, Category = Settings)
	static UPECustomSettings* GetCustomGameUserSettings();

	UFUNCTION(BlueprintCallable, Category = Settings)
	void SetAntiAliasingMode(const int InMode);

	UFUNCTION(BlueprintPure, Category = Settings)
	int GetAntiAliasingMode() const;

	UFUNCTION(BlueprintCallable, Category = Settings, meta = (DisplayName = "Set FSR Enabled"))
	void SetFSREnabled(const bool bEnable);

	UFUNCTION(BlueprintPure, Category = Settings, meta = (DisplayName = "Get FSR Enabled"))
	bool GetFSREnabled() const;

	UFUNCTION(BlueprintCallable, Category = Settings, meta = (DisplayName = "Set FSR Mode"))
	void SetFSRMode(const int InMode);

	UFUNCTION(BlueprintPure, Category = Settings, meta = (DisplayName = "Get FSR Mode"))
	int GetFSRMode() const;

	UFUNCTION(BlueprintCallable, Category = Settings)
	void SetTemporalUpscalingEnabled(const bool bEnable);

	UFUNCTION(BlueprintPure, Category = Settings)
	bool GetTemporalUpscalingEnabled() const;

private:
	UPROPERTY(config, meta = (Setter = "SetAntiAliasingMode", Getter = "GetAntiAliasingMode"))
	int AntiAliasingMode;

	UPROPERTY(config, meta = (Setter = "SetFSREnabled", Getter = "GetFSREnabled"))
	bool bFSREnabled;

	UPROPERTY(config, meta = (Setter = "SetFSRMode", Getter = "GetFSRMode"))
	int FSRMode;

	UPROPERTY(config, meta = (Setter = "SetTemporalUpscalingEnabled", Getter = "GetTemporalUpscalingEnabled"))
	bool bEnableTemporalUpscaling;
};
