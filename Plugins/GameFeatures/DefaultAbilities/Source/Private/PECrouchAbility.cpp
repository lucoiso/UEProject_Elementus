// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "PECrouchAbility.h"
#include "GameFramework/Character.h"

UPECrouchAbility::UPECrouchAbility(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.Default.Crouch"));
}

void UPECrouchAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ACharacter* const Player = Cast<ACharacter>(ActorInfo->AvatarActor.Get());

	// Only characters can activate this ability
	if (!IsValid(Player))
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	Player->CanCrouch() && !Player->bIsCrouched ? Player->Crouch() : Player->UnCrouch();

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
