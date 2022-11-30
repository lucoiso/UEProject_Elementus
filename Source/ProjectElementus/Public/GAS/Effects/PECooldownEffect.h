// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include <CoreMinimal.h>
#include <GameplayEffect.h>
#include "PECooldownEffect.generated.h"

/**
 * 
 */
UCLASS(Category = "Project Elementus | Classes")
class PROJECTELEMENTUS_API UPECooldownEffect : public UGameplayEffect
{
	GENERATED_BODY()
		
public:
	explicit UPECooldownEffect(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
