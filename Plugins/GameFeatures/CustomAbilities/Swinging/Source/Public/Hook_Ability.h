// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "GAS/System/GASGameplayAbility.h"
#include "Hook_Ability.generated.h"

/**
 *
 */
UCLASS(MinimalAPI, NotPlaceable, HideDropdown, Category = "Custom GAS | Abilities")
class UHook_Ability : public UGASGameplayAbility
{
	GENERATED_BODY()

public:
	UHook_Ability(const FObjectInitializer& ObjectInitializer);

private:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;

	virtual void WaitGameplayEvent_Callback_Implementation(FGameplayEventData Payload) override;

	virtual void WaitTargetData_Callback_Implementation(const FGameplayAbilityTargetDataHandle& TargetDataHandle) override;
};
