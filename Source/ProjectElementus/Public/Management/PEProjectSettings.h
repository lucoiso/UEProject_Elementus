// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include <CoreMinimal.h>
#include <Engine/DeveloperSettings.h>
#include "PEProjectSettings.generated.h"

class UGameplayEffect;

/**
 * 
 */
UCLASS(Config = Engine, DefaultConfig, Meta = (DisplayName = "Project Elementus"))
class PROJECTELEMENTUS_API UPEProjectSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	explicit UPEProjectSettings(const FObjectInitializer& ObjectInitializer);
	
	/* A Blueprint Widget class to use as HUD */
	UPROPERTY(GlobalConfig, EditAnywhere, Category = "UI", Meta = (DisplayName = "HUD UMG Class"))
	TSoftClassPtr<UUserWidget> HUDClass;

	/* Color of player character's mesh */
	UPROPERTY(GlobalConfig, EditAnywhere, Category = "Character | Appearance")
	FLinearColor PlayerColor;

	/* Color of bot character's mesh */
	UPROPERTY(GlobalConfig, EditAnywhere, Category = "Character | Appearance")
	FLinearColor BotColor;

	/* Gravity Multiplier that will be added to character's movement component */
	UPROPERTY(GlobalConfig, EditAnywhere, Category = "Character | Physics")
	float GravityMultiplier;

	/* Speed Multiplier that will be added to character's movement component */
	UPROPERTY(GlobalConfig, EditAnywhere, Category = "Character | Physics")
	float SpeedMultiplier;

	/* Jump Multiplier that will be added to character's movement component */
	UPROPERTY(GlobalConfig, EditAnywhere, Category = "Character | Physics")
	float JumpMultiplier;

	/* Air Control Multiplier that will be added to character's movement component */
	UPROPERTY(GlobalConfig, EditAnywhere, Category = "Character | Physics")
	float AirControlMultiplier;
};
