#pragma once

#include "CoreMinimal.h"
#include "GAS/System/GASGameplayAbility.h"
#include "Telekinesis_Ability.generated.h"

class UTelekinesisAbility_Task;
/**
 *
 */
UCLASS(MinimalAPI, NotPlaceable, HideDropdown, Category = "Custom GAS | Abilities")
class UTelekinesis_Ability : public UGASGameplayAbility
{
	GENERATED_BODY()

public:
	UTelekinesis_Ability(const FObjectInitializer& ObjectInitializer);

private:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	                             const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;

	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle,
	                          const FGameplayAbilityActorInfo* ActorInfo,
	                          const FGameplayAbilityActivationInfo ActivationInfo) override;

	virtual void WaitTagAdded_Callback_Implementation() override;

	TWeakObjectPtr<UTelekinesisAbility_Task> AbilityTask;
};
