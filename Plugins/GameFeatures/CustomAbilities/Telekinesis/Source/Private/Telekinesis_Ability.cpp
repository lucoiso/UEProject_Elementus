// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Telekinesis_Ability.h"
#include "TelekinesisAbility_Task.h"

#include "GAS/System/GASTrace.h"
#include "ThrowableActor.h"

UTelekinesis_Ability::UTelekinesis_Ability(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.Telekinesis.Grab"));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.Telekinesis.Grab"));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayEffect.Debuff.Regeneration.Block.Mana"));

	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag("State.CannotInteract"));

	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.Swinging.Hook"));
}

void UTelekinesis_Ability::ActivateAbility
(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	FGASTraceDataHandle AbilityTraceDataHandle;
	AbilityTraceDataHandle.bDoTick = false;
	AbilityTraceDataHandle.Type = EGASTraceType::Single;
	AbilityTraceDataHandle.FilterHandle.Filter->RequiredActorClass = AThrowableActor::StaticClass();

	DoAbilityLineTrace(AbilityTraceDataHandle, ActorInfo->AvatarActor.Get());

	if (!IsValid(AbilityTraceDataHandle.Hit.GetActor()) ||
		AbilityTraceDataHandle.Hit.Location == FVector(0.f))
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		RemoveCooldownEffect(ActorInfo->AbilitySystemComponent.Get());
		return;
	}

	AbilityTask = UTelekinesisAbility_Task::TelekinesisAbilityMovement(this, FName("TelekinesisTask"),
		AbilityTraceDataHandle.Hit.GetActor());
	AbilityTask->ReadyForActivation();

	FGameplayCueParameters Params;
	Params.Location = AbilityTraceDataHandle.Hit.Location;
	Params.TargetAttachComponent = AbilityTraceDataHandle.Hit.GetComponent();

	ActivateGameplayCues(FGameplayTag::RequestGameplayTag("GameplayCue.Telekinesis.Grab"), Params,
		ActorInfo->AbilitySystemComponent.Get());

	ActivateWaitAddedTagTask(FGameplayTag::RequestGameplayTag("GameplayAbility.Telekinesis.Throw"));
}

void UTelekinesis_Ability::InputPressed(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);

	if (!IsActive())
	{
		return;
	}

	CancelAbility(Handle, ActorInfo, ActivationInfo, true);
}

void UTelekinesis_Ability::WaitAddedTag_Callback_Implementation()
{
	AbilityTask->ThrowObject();

	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}