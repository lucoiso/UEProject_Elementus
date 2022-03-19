// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Hook_Ability.h"
#include "HookAbility_Task.h"

#include "Actors/Character/PECharacterBase.h"
#include "GAS/System/GASTrace.h"

UHook_Ability::UHook_Ability(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.Swinging.Hook"));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.Swinging.Hook"));

	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayEffect.Debuff.Regeneration.Block.Stamina"));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayEffect.Debuff.Regeneration.Block.Mana"));

	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.Telekinesis.Grab"));
}

void UHook_Ability::ActivateAbility
(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	FGASTraceDataHandle AbilityTraceDataHandle;
	AbilityTraceDataHandle.bDoTick = false;
	AbilityTraceDataHandle.Type = EGASTraceType::Single;

	DoAbilityLineTrace(AbilityTraceDataHandle, ActorInfo->AvatarActor.Get());

	if (!IsValid(AbilityTraceDataHandle.Hit.GetActor()) ||
		AbilityTraceDataHandle.Hit.Location == FVector(0.f))
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	UHookAbility_Task* AbilityTask = UHookAbility_Task::HookAbilityMovement(
		this, FName("HookTask"), AbilityTraceDataHandle.Hit);
	AbilityTask->ReadyForActivation();

	FGameplayCueParameters Params;
	Params.Location = AbilityTraceDataHandle.Hit.Location;
	Params.TargetAttachComponent = AbilityTraceDataHandle.Hit.GetComponent();

	ActivateGameplayCues(FGameplayTag::RequestGameplayTag("GameplayCue.Swinging.Hook"), Params,
		ActorInfo->AbilitySystemComponent.Get());

	if (Cast<APECharacterBase>(AbilityTraceDataHandle.Hit.GetActor()))
	{
		FTimerDelegate TimerDelegate;
		FTimerHandle TimerHandle;
		TimerDelegate.BindLambda([this, Handle, ActorInfo, ActivationInfo]() -> void
			{
				EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
			});

		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, AbilityActiveTime, false);
	}
}

void UHook_Ability::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}