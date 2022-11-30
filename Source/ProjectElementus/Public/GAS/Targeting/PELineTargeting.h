// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include <CoreMinimal.h>
#include <Abilities/GameplayAbilityTargetActor_SingleLineTrace.h>
#include "PELineTargeting.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, Category = "Project Elementus | Classes")
class PROJECTELEMENTUS_API APELineTargeting : public AGameplayAbilityTargetActor_SingleLineTrace
{
	GENERATED_BODY()

public:
	explicit APELineTargeting(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
