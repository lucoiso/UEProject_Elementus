// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

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

	virtual void WaitTargetData_Callback_Implementation(const FGameplayAbilityTargetDataHandle& TargetDataHandle) override;

	UFUNCTION()
		void GrabbingComplete(const bool ValidTarget);

	virtual void WaitConfirmInput_Callback_Implementation() override;

	virtual void WaitMontage_Callback_Implementation() override;

	virtual void WaitGameplayEvent_Callback_Implementation(FGameplayEventData Payload) override;

	TWeakObjectPtr<UTelekinesisAbility_Task> AbilityTask;
};
