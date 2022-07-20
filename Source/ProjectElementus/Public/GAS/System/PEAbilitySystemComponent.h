// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GAS/System/PEAbilityData.h"
#include "PEAbilitySystemComponent.generated.h"

/**
 *
 */
UCLASS(NotBlueprintable, NotPlaceable, Category = "Project Elementus | Classes")
class PROJECTELEMENTUS_API UPEAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	explicit UPEAbilitySystemComponent(const FObjectInitializer& ObjectInitializer);

	/* Apply a grouped GE data to self Ability System Component */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	void ApplyEffectGroupedDataToSelf(FGameplayEffectGroupedData GroupedData);

	/* Apply a grouped GE data to target Ability System Component */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	void ApplyEffectGroupedDataToTarget(FGameplayEffectGroupedData GroupedData, UAbilitySystemComponent* TargetABSC);

	template <typename T>
	const T* GetCustomAttributeSet() const
	{
		return Cast<T>(GetAttributeSet(T::StaticClass()));
	}
};
