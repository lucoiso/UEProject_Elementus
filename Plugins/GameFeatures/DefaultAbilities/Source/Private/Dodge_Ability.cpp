// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Dodge_Ability.h"
// #include "Abilities/Tasks/AbilityTask_ApplyRootMotionMoveToForce.h"
#include "Actors/Character/PECharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UDodge_Ability::UDodge_Ability(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.Default.Dodge"));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.Default.Dodge"));
}

void UDodge_Ability::ActivateAbility
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
	if (Player->GetCharacterMovement()->IsFalling())
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	/*
	FVector Location =
		FVector(Player->GetVelocity().X, Player->GetVelocity().Y, 0.f).Size() == 0 ?
		Player->GetActorForwardVector() * -1.f :
		Player->GetLastMovementInputVector();

	Location = Player->GetActorLocation() + Location * AbilityMaxRange + FVector(0.f, 0.f, 1.25f);

	UAbilityTask_ApplyRootMotionMoveToForce* MoveTask =
		UAbilityTask_ApplyRootMotionMoveToForce::ApplyRootMotionMoveToForce(this,
			"Dodge_Task",
			Location,
			AbilityActiveTime,
			false,
			EMovementMode::MOVE_None,
			false,
			nullptr,
			ERootMotionFinishVelocityMode::SetVelocity,
			Player->GetVelocity(),
			300.f);

	MoveTask->ReadyForActivation();
	*/

	ApplyAbilityEffectsToSelf(Handle, ActorInfo, ActivationInfo);
}
