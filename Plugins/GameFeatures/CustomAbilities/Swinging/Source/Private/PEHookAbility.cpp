// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "PEHookAbility.h"
#include "PEHookAbility_Task.h"
#include "Actors/Character/PECharacterBase.h"
#include "Abilities/GameplayAbilityTargetActor_SingleLineTrace.h"

UPEHookAbility::UPEHookAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.Swinging"));

	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.Swinging"));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayEffect.Debuff.Regeneration.Block.Stamina"));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayEffect.Debuff.Regeneration.Block.Mana"));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag("State.BlockLeftHand"));

	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag("State.BlockLeftHand"));
}

void UPEHookAbility::ActivateAbility
(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ActivateWaitMontageTask(NAME_None, 1.25f);
	ActivateWaitGameplayEventTask(FGameplayTag::RequestGameplayTag("Data.Notify.Ability"));
}

void UPEHookAbility::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UPEHookAbility::WaitGameplayEvent_Callback_Implementation(FGameplayEventData Payload)
{
	FTargetActorSpawnParams TargetingParams;
	TargetingParams.StartLocation = MakeTargetLocationInfoFromOwnerSkeletalMeshComponent("hand_l");

	ActivateWaitTargetDataTask(EGameplayTargetingConfirmation::Instant, AGameplayAbilityTargetActor_SingleLineTrace::StaticClass(), TargetingParams);
}

void UPEHookAbility::WaitTargetData_Callback_Implementation(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (!TargetDataHandle.IsValid(0))
	{
		CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
		return;
	}

	const FGameplayAbilityTargetData* TargetData = TargetDataHandle.Get(0);
	const FHitResult* TargetHit = TargetData->GetHitResult();

	if (!IsValid(TargetHit->GetActor()))
	{
		CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
		return;
	}

	UPEHookAbility_Task* AbilityTask = UPEHookAbility_Task::HookAbilityMovement(
		this, FName("HookTask"), *TargetHit);

	AbilityTask->ReadyForActivation();

	FGameplayCueParameters Params;
	Params.Location = TargetHit->Location;
	Params.TargetAttachComponent = TargetHit->GetComponent();

	TargetData->AddTargetDataToGameplayCueParameters(Params);

	ActivateGameplayCues(FGameplayTag::RequestGameplayTag("GameplayCue.Swinging"), Params,
		GetCurrentActorInfo()->AbilitySystemComponent.Get());

	if (Cast<APECharacterBase>(TargetHit->GetActor()))
	{
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda([=]() -> void
			{
				if (IsActive())
				{
					EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
				}
			});

		GetWorld()->GetTimerManager().SetTimer(CancelationTimerHandle, TimerDelegate, AbilityActiveTime, false);
	}
}