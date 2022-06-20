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

	// Only characters can activate this ability
	if (!IsValid(Player))
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	// Check if the player is in air and launch him (second jump) or just do a normal jump (first jump)
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

void UPEDoubleJumpAbility::InputReleased(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo,
                                         const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	// Send the StopJumping event to the player if valid
	if (APECharacter* Player = Cast<APECharacter>(ActorInfo->AvatarActor.Get()))
	{
		Player->StopJumping();
	}
}
