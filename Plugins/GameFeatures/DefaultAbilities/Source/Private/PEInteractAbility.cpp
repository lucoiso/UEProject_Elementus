// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "PEInteractAbility.h"
#include "Tasks/PEInteractAbility_Task.h"
#include <Actors/Character/PECharacter.h>
#include <Actors/Interfaces/PEInteractable.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(PEInteractAbility)

UPEInteractAbility::UPEInteractAbility(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.Default.Interact"));
	bAutoActivateOnGrant = true;
	bWaitCancel = false;

	AbilityMaxRange = 1000.f;
	bUseCustomDepth = false;

	bReplicateInputDirectly = true;
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

	if (TaskHandle.IsValid() && TaskHandle->GetIsInteractAllowed())
	{
		if (IsValid(TaskHandle->GetInteractable()) && IPEInteractable::Execute_IsInteractEnabled(TaskHandle->GetInteractable()))
		{
			IPEInteractable::Execute_DoInteractionBehavior(TaskHandle->GetInteractable(), Cast<APECharacter>(ActorInfo->AvatarActor.Get()), TaskHandle->GetInteractableHitResult());
		}
	}
}
