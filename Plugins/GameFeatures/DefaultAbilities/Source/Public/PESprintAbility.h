// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "GAS/System/PEGameplayAbility.h"
#include "PESprintAbility.generated.h"

/**
 *
 */
UCLASS(MinimalAPI, NotPlaceable, HideDropdown, Category = "Project Elementus | Classes")
class UPESprintAbility final : public UPEGameplayAbility
{
	GENERATED_BODY()

public:
	explicit UPESprintAbility(const FObjectInitializer& ObjectInitializer);

private:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	                             const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle,
	                           const FGameplayAbilityActorInfo* ActorInfo,
	                           const FGameplayAbilityActivationInfo ActivationInfo) override;
};
