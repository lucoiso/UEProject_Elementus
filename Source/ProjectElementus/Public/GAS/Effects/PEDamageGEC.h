// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "PEDamageGEC.generated.h"

/**
 *
 */
UCLASS(NotBlueprintable, NotPlaceable, Category = "Project Elementus | Classes")
class PROJECTELEMENTUS_API UPEDamageGEC final : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	explicit UPEDamageGEC(const FObjectInitializer& ObjectInitializer);

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	                                    OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
