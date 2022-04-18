// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Management/Data/GASAbilityData.h"
#include "GASAbilitySystemComponent.generated.h"

/**
 *
 */
UCLASS(NotBlueprintable, NotPlaceable, Category = "Custom GAS | Abilities")
class PROJECTELEMENTUS_API UGASAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UGASAbilitySystemComponent(const FObjectInitializer& ObjectInitializer);

private:
	bool bStartupEffectsApplied = false;
	bool bCharacterAbilitiesGiven = false;

public:
	FORCEINLINE const bool IsStartupEffectAlreadyApplied() const
	{
		return bStartupEffectsApplied;
	};

	FORCEINLINE const bool IsAbilitiesAlreadyGiven() const
	{
		return bCharacterAbilitiesGiven;
	};

	FORCEINLINE void SetStartupEffectApplied(const bool NewValue)
	{
		bStartupEffectsApplied = NewValue;
	};

	FORCEINLINE void SetAbilitiesGiven(const bool NewValue)
	{
		bCharacterAbilitiesGiven = NewValue;
	};

	/* Apply a grouped GE data to self Ability System Component */
	UFUNCTION(BlueprintCallable, Category = "Custom GAS | Management")
		void ApplyEffectGroupedDataToSelf(FGameplayEffectGroupedData GroupedData);

	/* Apply a grouped GE data to target Ability System Component */
	UFUNCTION(BlueprintCallable, Category = "Custom GAS | Management")
		void ApplyEffectGroupedDataToTarget(FGameplayEffectGroupedData GroupedData, UAbilitySystemComponent* TargetABSC);
};
