#include "DoubleJump_Ability.h"
#include "Actors/Character/PECharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UDoubleJump_Ability::UDoubleJump_Ability(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.Default.DoubleJump"));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.Default.DoubleJump"));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayEffect.Debuff.Regeneration.Block.Stamina"));

	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.Default.Dash"));
}

void UDoubleJump_Ability::ActivateAbility
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

	if (!Player->GetCharacterMovement()->IsFalling())
	{
		Player->Jump();
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	Player->LaunchCharacter(FVector(0.f, 0.f, AbilityMaxRange), false, true);

	// ActivateGameplayCues(FGameplayTag::RequestGameplayTag("GameplayCue.Default.DoubleJump"), FGameplayCueParameters(),
	// ActorInfo->AbilitySystemComponent.Get());
}
