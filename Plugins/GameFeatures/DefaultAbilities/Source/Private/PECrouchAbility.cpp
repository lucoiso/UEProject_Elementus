// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "PECrouchAbility.h"
#include "Actors/Character/PECharacterBase.h"

UPECrouchAbility::UPECrouchAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;
}

void UPECrouchAbility::ActivateAbility
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

	Player->CanCrouch() && !Player->bIsCrouched ? Player->Crouch() : Player->UnCrouch();

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}