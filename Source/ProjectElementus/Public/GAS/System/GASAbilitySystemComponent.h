#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
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
	FORCEINLINE bool IsStartupEffectAlreadyApplied() const
	{
		return bStartupEffectsApplied;
	};

	FORCEINLINE bool IsAbilitiesAlreadyGiven() const
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
};
