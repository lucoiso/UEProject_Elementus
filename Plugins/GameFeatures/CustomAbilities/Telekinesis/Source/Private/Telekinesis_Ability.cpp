// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Telekinesis_Ability.h"
#include "TelekinesisAbility_Task.h"
#include "ThrowableActor.h"
#include "Abilities/GameplayAbilityTargetActor_SingleLineTrace.h"

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

	FTargetActorSpawnParams TargetingParams;
	TargetingParams.TargetFilter.RequiredActorClass = AThrowableActor::StaticClass();
	TargetingParams.StartLocation = MakeTargetLocationInfoFromOwnerSkeletalMeshComponent("hand_l");

	ActivateWaitTargetDataTask(EGameplayTargetingConfirmation::Instant, AGameplayAbilityTargetActor_SingleLineTrace::StaticClass(), TargetingParams);
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

void UTelekinesis_Ability::WaitTargetData_Callback_Implementation(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (!TargetDataHandle.IsValid(0))
	{
		RemoveCooldownEffect(GetCurrentActorInfo()->AbilitySystemComponent.Get());
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

	AbilityTask = UTelekinesisAbility_Task::TelekinesisAbilityMovement(this, FName("TelekinesisTask"),
		TargetHit->GetActor());
	AbilityTask->ReadyForActivation();

	FGameplayCueParameters Params;
	Params.Location = TargetHit->Location;
	Params.TargetAttachComponent = TargetHit->GetComponent();

	ActivateGameplayCues(FGameplayTag::RequestGameplayTag("GameplayCue.Telekinesis.Grab"), Params,
		GetCurrentActorInfo()->AbilitySystemComponent.Get());

	ActivateWaitAddedTagTask(FGameplayTag::RequestGameplayTag("GameplayAbility.Telekinesis.Throw"));
}