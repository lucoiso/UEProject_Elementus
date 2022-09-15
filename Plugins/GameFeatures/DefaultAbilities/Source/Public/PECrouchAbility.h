// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "GAS/System/PEGameplayAbility.h"
#include "PECrouchAbility.generated.h"

/**
 *
 */
UCLASS(MinimalAPI, NotPlaceable, HideDropdown, Category = "Project Elementus | Classes")
class UPECrouchAbility final : public UPEGameplayAbility
{
	GENERATED_BODY()

public:
	explicit UPECrouchAbility(const FObjectInitializer& ObjectInitializer);

private:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
