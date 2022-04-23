// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "GAS/System/PEGameplayAbility.h"
#include "PETelekinesisAbility.generated.h"

class UPETelekinesisAbility_Task;
/**
 *
 */
UCLASS(MinimalAPI, NotPlaceable, HideDropdown, Category = "Custom GAS | Abilities")
class UPETelekinesisAbility : public UPEGameplayAbility
{
	GENERATED_BODY()

public:
	UPETelekinesisAbility(const FObjectInitializer& ObjectInitializer);

private:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;

	virtual void
		WaitTargetData_Callback_Implementation(const FGameplayAbilityTargetDataHandle& TargetDataHandle) override;

	UFUNCTION()
		void GrabbingComplete(const bool ValidTarget);

	virtual void WaitConfirmInput_Callback_Implementation() override;

	virtual void WaitGameplayEvent_Callback_Implementation(FGameplayEventData Payload) override;

	TWeakObjectPtr<UPETelekinesisAbility_Task> AbilityTask;
};
