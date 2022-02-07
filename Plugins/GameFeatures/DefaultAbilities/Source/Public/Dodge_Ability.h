#pragma once

#include "CoreMinimal.h"
#include "GAS/System/GASGameplayAbility.h"
#include "Dodge_Ability.generated.h"

/**
 *
 */
UCLASS(MinimalAPI, NotPlaceable, HideDropdown, Category = "Custom GAS | Abilities")
class UDodge_Ability : public UGASGameplayAbility
{
	GENERATED_BODY()

public:
	UDodge_Ability(const FObjectInitializer& ObjectInitializer);

private:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	                             const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;
};
