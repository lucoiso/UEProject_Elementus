#include "Dash_Ability.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "Actors/Character/PECharacterBase.h"

UDash_Ability::UDash_Ability(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.Default.Dash"));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.Default.Dash"));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayEffect.Debuff.Regeneration.Block.Stamina"));

	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.Swinging.Hook"));
}

void UDash_Ability::ActivateAbility
(const FGameplayAbilitySpecHandle Handle,
 const FGameplayAbilityActorInfo* ActorInfo,
 const FGameplayAbilityActivationInfo ActivationInfo,
 const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	const APECharacterBase* Player = Cast<APECharacterBase>(ActorInfo->AvatarActor.Get());

	if (!IsValid(Player))
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	const FVector DashDirection =
		FVector(Player->GetVelocity().X, Player->GetVelocity().Y, 0.f).Size() == 0
			? Player->GetCameraForwardVector()
			: Player->GetLastMovementInputVector();

	const FVector PrevVelocity = Player->GetVelocity();

	UAbilityTask_ApplyRootMotionConstantForce* MoveTask =
		UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(this,
			"Dash_Task",
			DashDirection,
			AbilityMaxRange,
			AbilityActiveTime,
			false,
			nullptr,
			ERootMotionFinishVelocityMode::SetVelocity,
			PrevVelocity,
			0.f,
			false);

	MoveTask->ReadyForActivation();

	// ActivateGameplayCues(FGameplayTag::RequestGameplayTag("GameplayCue.Default.Dash"), FGameplayCueParameters(),
	// ActorInfo->AbilitySystemComponent.Get());
}
