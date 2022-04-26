// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "PEDoubleJumpAbility.h"
#include "Actors/Character/PECharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UPEDoubleJumpAbility::UPEDoubleJumpAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.Default.DoubleJump"));

	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayEffect.Debuff.Regeneration.Block.Stamina"));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.Default.Dash"));
}

void UPEDoubleJumpAbility::ActivateAbility
(const FGameplayAbilitySpecHandle Handle,
 const FGameplayAbilityActorInfo* ActorInfo,
 const FGameplayAbilityActivationInfo ActivationInfo,
 const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	APECharacter* Player = Cast<APECharacter>(ActorInfo->AvatarActor.Get());

	if (!IsValid(Player))
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}
	if (!Player->GetCharacterMovement()->IsFalling())
	{
		Player->Jump();
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
	else
	{
		Player->LaunchCharacter(FVector(0.f, 0.f, AbilityMaxRange), false, true);
	}
}
