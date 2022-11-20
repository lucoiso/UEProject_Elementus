// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "PEDevSettings.generated.h"

/**
 * 
 */
UCLASS(Config = Game, DefaultConfig, Meta = (DisplayName = "Project Elementus"))
class PROJECTELEMENTUS_API UPEDevSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	explicit UPEDevSettings(const FObjectInitializer& ObjectInitializer);

	/* A Blueprint Widget class to use as HUD */
	UPROPERTY(GlobalConfig, EditAnywhere, Category = "UI", Meta = (DisplayName = "HUD UMG Class"))
	TSoftClassPtr<UUserWidget> HUDClass;

	/* A Blueprint Widget class to use as Inventory UI */
	UPROPERTY(GlobalConfig, EditAnywhere, Category = "UI", Meta = (DisplayName = "Main Inventory UMG Class"))
	TSoftClassPtr<UUserWidget> MainInventoryWidget;

	/* A Blueprint Widget class to use as Trade UI */
	UPROPERTY(GlobalConfig, EditAnywhere, Category = "UI", Meta = (DisplayName = "Trade Inventory UMG Class"))
	TSoftClassPtr<UUserWidget> TradeInventoryWidget;

	/* Color of player's character mesh */
	UPROPERTY(GlobalConfig, EditAnywhere, Category = "Character | Appearance")
	FLinearColor PlayerColor;

	/* Color of bot's character mesh */
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

	/* Global death effect used by GAS objects */
	UPROPERTY(GlobalConfig, EditAnywhere, Category = "GAS | Effects")
	TSoftClassPtr<class UGameplayEffect> GlobalDeathEffect;

	/* Global stun effect used by GAS objects */
	UPROPERTY(GlobalConfig, EditAnywhere, Category = "GAS | Effects")
	TSoftClassPtr<class UGameplayEffect> GlobalStunEffect;
};
