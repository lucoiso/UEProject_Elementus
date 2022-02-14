// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Crouch_Ability.h"
#include "Actors/Character/PECharacterBase.h"

UCrouch_Ability::UCrouch_Ability(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.Default.Crouch"));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.Default.Crouch"));
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
	if (!Player->CanCrouch())
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		if (Player->bIsCrouched)
		{
			Player->UnCrouch();
		}

		return;
	}

	Player->Crouch();
}

void UCrouch_Ability::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);

	if (!IsActive())
	{
		return;
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);

	APECharacterBase* Player = Cast<APECharacterBase>(ActorInfo->AvatarActor.Get());

	if (IsValid(Player) && Player->bIsCrouched)
	{
		Player->UnCrouch();
	}
}
