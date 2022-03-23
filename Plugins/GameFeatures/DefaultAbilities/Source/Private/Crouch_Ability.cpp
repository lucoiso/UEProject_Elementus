// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Crouch_Ability.h"
#include "Actors/Character/PECharacterBase.h"

UCrouch_Ability::UCrouch_Ability(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;
}

void UCrouch_Ability::ActivateAbility
(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	APECharacterBase* Player = Cast<APECharacterBase>(ActorInfo->AvatarActor.Get());

	if (!IsValid(Player))
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}
	else if (Player->CanCrouch())
	{
		Player->bIsCrouched ?
			Player->UnCrouch() :
			Player->Crouch();
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}