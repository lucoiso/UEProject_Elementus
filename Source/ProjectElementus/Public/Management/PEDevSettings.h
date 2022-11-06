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
UCLASS(Config = Game, DefaultConfig, Meta = (DisplayName = "Project Elementus Settings"))
class PROJECTELEMENTUS_API UPEDevSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	explicit UPEDevSettings(const FObjectInitializer& ObjectInitializer);
};
