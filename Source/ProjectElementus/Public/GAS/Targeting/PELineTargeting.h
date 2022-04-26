// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor_SingleLineTrace.h"
#include "PELineTargeting.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, Category = "Custom GAS | Actors")
class PROJECTELEMENTUS_API APELineTargeting final : public AGameplayAbilityTargetActor_SingleLineTrace
{
	GENERATED_BODY()

public:
	explicit APELineTargeting(const FObjectInitializer& ObjectInitializer);
};
