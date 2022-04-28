// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "PETelekinesisAbility.h"
#include "PETelekinesisAbility_Task.h"
#include "PEThrowableActor.h"
#include "GAS/Targeting/PELineTargeting.h"

UPETelekinesisAbility::UPETelekinesisAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	  ThrowIntensity(2750)
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.Telekinesis"));

	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayEffect.Debuff.Regeneration.Block.Mana"));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag("State.CannotInteract"));
}

void UPETelekinesisAbility::ActivateAbility
(const FGameplayAbilitySpecHandle Handle,
 const FGameplayAbilityActorInfo* ActorInfo,
 const FGameplayAbilityActivationInfo ActivationInfo,
 const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	FTargetActorSpawnParams TargetingParams;
	TargetingParams.TargetFilter.RequiredActorClass = APEThrowableActor::StaticClass();
	TargetingParams.StartLocation = MakeTargetLocationInfoFromOwnerSkeletalMeshComponent("head");

	ActivateWaitTargetDataTask(EGameplayTargetingConfirmation::Instant,
	                           APELineTargeting::StaticClass(), TargetingParams);
}

void UPETelekinesisAbility::InputPressed(const FGameplayAbilitySpecHandle Handle,
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

void UPETelekinesisAbility::WaitTargetData_Callback_Implementation(
	const FGameplayAbilityTargetDataHandle& TargetDataHandle)
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
		RemoveCooldownEffect(GetCurrentActorInfo()->AbilitySystemComponent.Get());
		return;
	}

	AbilityTask = UPETelekinesisAbility_Task::PETelekinesisAbilityMovement(
		this, FName("TelekinesisTask"), ThrowIntensity, TargetHit->GetActor());

	AbilityTask->OnGrabbing.BindDynamic(this, &UPETelekinesisAbility::GrabbingComplete);

	AbilityTask->ReadyForActivation();

	FGameplayCueParameters Params;
	Params.Location = TargetHit->Location;
	Params.TargetAttachComponent = TargetHit->GetComponent();

	TargetData->AddTargetDataToGameplayCueParameters(Params);

	ActivateGameplayCues(FGameplayTag::RequestGameplayTag("GameplayCue.Telekinesis"), Params,
	                     GetCurrentActorInfo()->AbilitySystemComponent.Get());
}

void UPETelekinesisAbility::GrabbingComplete(const bool ValidTarget)
{
	if (ValidTarget)
	{
		ActivateWaitConfirmInputTask();
		ActivateWaitGameplayEventTask(FGameplayTag::RequestGameplayTag("Data.Notify.Ability"));
	}
	else
	{
		CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
		RemoveCooldownEffect(GetCurrentActorInfo()->AbilitySystemComponent.Get());
	}
}

void UPETelekinesisAbility::WaitConfirmInput_Callback_Implementation()
{
	ActivateWaitMontageTask();
}

void UPETelekinesisAbility::WaitGameplayEvent_Callback_Implementation(FGameplayEventData Payload)
{
	AbilityTask->ThrowObject();

	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}
