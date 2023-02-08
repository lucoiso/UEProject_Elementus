// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "PETelekinesisAbility.h"
#include "PETelekinesisAbility_Task.h"
#include "PEThrowableActor.h"
#include <GAS/Targeting/PELineTargeting.h>
#include <GAS/System/PETrace.h>
#include <Management/Data/PEGlobalTags.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(PETelekinesisAbility)

UPETelekinesisAbility::UPETelekinesisAbility(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), ThrowIntensity(2750.f)
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.Telekinesis"));

	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(GlobalTag_RegenBlock_Mana));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(GlobalTag_CannotInteract));

	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(GlobalTag_WeaponSlot_Base));
}

void UPETelekinesisAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// Targeting: Params	
	FPETargetActorSpawnParams TargetingParams;
	TargetingParams.TargetFilter.RequiredActorClass = APEThrowableActor::StaticClass();
	TargetingParams.StartLocation = MakeTargetLocationInfoFromOwnerSkeletalMeshComponent("head");

	// Targeting: Start task	
	ActivateWaitTargetDataTask(EGameplayTargetingConfirmation::Instant, APELineTargeting::StaticClass(), TargetingParams);
}

void UPETelekinesisAbility::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);

	if (!IsActive())
	{
		return;
	}

	CancelAbility(Handle, ActorInfo, ActivationInfo, true);
}

void UPETelekinesisAbility::WaitTargetData_Callback_Implementation(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	// If target is invalid, end the ability
	if (!TargetDataHandle.IsValid(0))
	{
		CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
		RemoveCooldownEffect(GetCurrentActorInfo()->AbilitySystemComponent.Get());
		return;
	}

	// Get the first target only since this is a single target ability
	const FGameplayAbilityTargetData* const TargetData = TargetDataHandle.Get(0);
	const FHitResult* const TargetHit = TargetData->GetHitResult();

	// If there's no actor at the target data, end the ability: Invalid Target
	if (!IsValid(TargetHit->GetActor()))
	{
		CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
		RemoveCooldownEffect(GetCurrentActorInfo()->AbilitySystemComponent.Get());
		return;
	}

	// Create the telekinesis movement task:
	// This task will perform the physical grabbing movement on target
	AbilityTask = UPETelekinesisAbility_Task::PETelekinesisAbilityMovement(this, TEXT("TelekinesisTask"), ThrowIntensity, TargetHit->GetActor());

	// When the grabbing task returns a result, will call GrabbingComplete function
	AbilityTask->OnGrabbing.BindDynamic(this, &UPETelekinesisAbility::GrabbingComplete);

	AbilityTask->ReadyForActivation();

	FGameplayCueParameters Params;
	Params.Location = TargetHit->Location;
	Params.TargetAttachComponent = TargetHit->GetComponent();

	TargetData->AddTargetDataToGameplayCueParameters(Params);

	ActivateGameplayCues(FGameplayTag::RequestGameplayTag("GameplayCue.Telekinesis"), Params);
}

void UPETelekinesisAbility::GrabbingComplete(const bool ValidTarget)
{
	// Check if the telekinesis task returned a valid target and if true
	// initialize the Wait Confirm Input task and Wait Gameplay Event:
	// When the AnimNotify is triggered, will launch the grabbed actor in the direction of the camera
	if (ValidTarget)
	{
		ActivateWaitConfirmInputTask();
		ActivateWaitGameplayEventTask(FGameplayTag::RequestGameplayTag(GlobalTag_AbilityNotify));
	}
	else
	{
		CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
		RemoveCooldownEffect(GetCurrentActorInfo()->AbilitySystemComponent.Get());
	}
}

void UPETelekinesisAbility::WaitConfirmInput_Callback_Implementation()
{
	// Play a animation montage
	ActivateWaitMontageTask();
}

void UPETelekinesisAbility::WaitGameplayEvent_Callback_Implementation(FGameplayEventData Payload)
{
	if (IsValid(AbilityTask->GetTelekinesisTarget()))
	{
		PlayAbilitySoundAttached(AbilityTask->GetTelekinesisTarget()->GetRootComponent());

		// When the AnimNotify is triggered, will launch the grabbed actor
		// in the direction of the camera and end the ability
		AbilityTask->ThrowObject();
	}
	
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}
