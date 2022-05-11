// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor_GroundTrace.h"
#include "PEGroundTargeting.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, Category = "Project Elementus | Classes")
class PROJECTELEMENTUS_API APEGroundTargeting final : public AGameplayAbilityTargetActor_GroundTrace
{
	GENERATED_BODY()

public:
	explicit APEGroundTargeting(const FObjectInitializer& ObjectInitializer);
};
