// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Interact_Ability.h"
#include "Actors/Character/PECharacterBase.h"
#include "GAS/System/GASAbilitySystemComponent.h"

UInteract_Ability::UInteract_Ability(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.Default.Interact"));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.Default.Interact"));

	ActivationRequiredTags.AddTag(FGameplayTag::RequestGameplayTag("State.CanInteract"));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag("State.CannotInteract"));
}

void UInteract_Ability::ActivateAbility
(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// TO DO

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}