// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "GAS/System/PEGameplayAbility.h"
#include "PEHookAbility.generated.h"

class UPEHookAbility_Task;
/**
 *
 */
UCLASS(MinimalAPI, NotPlaceable, HideDropdown, Category = "Project Elementus | Classes")
class UPEHookAbility final : public UPEGameplayAbility
{
	GENERATED_BODY()

public:
	explicit UPEHookAbility(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties")
	float HookIntensity;

private:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	                             const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle,
	                           const FGameplayAbilityActorInfo* ActorInfo,
	                           const FGameplayAbilityActivationInfo ActivationInfo) override;

	virtual void WaitGameplayEvent_Callback_Implementation(FGameplayEventData Payload) override;

	virtual void
	WaitTargetData_Callback_Implementation(const FGameplayAbilityTargetDataHandle& TargetDataHandle) override;

	virtual void WaitConfirmInput_Callback_Implementation() override;

	TWeakObjectPtr<UPEHookAbility_Task> TaskHandle;
};
