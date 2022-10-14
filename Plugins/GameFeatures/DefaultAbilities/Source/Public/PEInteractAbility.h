// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "GAS/System/PEGameplayAbility.h"
#include "Tasks/PEInteractAbility_Task.h"
#include "PEInteractAbility.generated.h"

/**
 *
 */
UCLASS(MinimalAPI, NotPlaceable, HideDropdown, Category = "Project Elementus | Classes")
class UPEInteractAbility final : public UPEGameplayAbility
{
	GENERATED_BODY()

public:
	explicit UPEInteractAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/* Activate/Deactivate custom depth usage
	 * This is useful if you're using a post process material to apply outlines/highlights */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties")
	bool bUseCustomDepth;

private:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	TWeakObjectPtr<UPEInteractAbility_Task> TaskHandle;
};
