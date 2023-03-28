// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "PEInteractAbility.h"
#include "Tasks/PEInteractAbility_Task.h"
#include <GameFramework/Character.h>
#include <Interfaces/PEInteractable.h>
#include <AbilitySystemComponent.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(PEInteractAbility)

UPEInteractAbility::UPEInteractAbility(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.Default.Interact"));
	bAutoActivateOnGrant = true;
	bWaitCancel = false;

	AbilityMaxRange = 1000.f;
	bUseCustomDepth = false;
}

void UPEInteractAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	TaskHandle = UPEInteractAbility_Task::InteractionTask(this, TEXT("InteractTask"), AbilityMaxRange, bUseCustomDepth);
	TaskHandle->ReadyForActivation();
}

void UPEInteractAbility::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);

	if (!TaskHandle.IsValid())
	{
		return;
	}

	TaskHandle->UpdateInteractableTarget();

	const bool bCanInteract = TaskHandle->GetIsInteractAllowed() && IsValid(TaskHandle->GetInteractable()) && IPEInteractable::Execute_IsInteractEnabled(TaskHandle->GetInteractable());

	if (bCanInteract)
	{
		if (!ActorInfo->IsNetAuthority())
		{
			ActorInfo->AbilitySystemComponent->ServerSetInputPressed(Handle);
		}

		IPEInteractable::Execute_DoInteractionBehavior(TaskHandle->GetInteractable(), Cast<ACharacter>(ActorInfo->AvatarActor.Get()), TaskHandle->GetInteractableHitResult());
	}
}
