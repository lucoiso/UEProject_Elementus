// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "PETelekinesisAbility.h"
#include "PETelekinesisAbility_Task.h"
#include "PEThrowableActor.h"
#include "GAS/Targeting/PELineTargeting.h"
#include "Kismet/GameplayStatics.h"

UPETelekinesisAbility::UPETelekinesisAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	  ThrowIntensity(2750.f)
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.Telekinesis"));

	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayEffect.Debuff.Regeneration.Block.Mana"));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag("State.CannotInteract"));

	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Weapon.Equipped")));

	static const ConstructorHelpers::FObjectFinder<USoundBase> ImpulseSound_ObjRef(
		TEXT("/Telekinesis/Sounds/MS_Impulse"));
	if constexpr (&ImpulseSound_ObjRef.Object != nullptr)
	{
		ImpulseSound = ImpulseSound_ObjRef.Object;
	}
}

void UPETelekinesisAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilityActivationInfo ActivationInfo,
                                            const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// Targeting: Params	
	FTargetActorSpawnParams TargetingParams;
	TargetingParams.TargetFilter.RequiredActorClass = APEThrowableActor::StaticClass();
	TargetingParams.StartLocation = MakeTargetLocationInfoFromOwnerSkeletalMeshComponent("head");

	// Targeting: Start task	
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
	// If target is invalid, end the ability
	if (!TargetDataHandle.IsValid(0))
	{
		CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
		RemoveCooldownEffect(GetCurrentActorInfo()->AbilitySystemComponent.Get());
		return;
	}

	// Get the first target only since this is a single target ability
	const FGameplayAbilityTargetData* TargetData = TargetDataHandle.Get(0);
	const FHitResult* TargetHit = TargetData->GetHitResult();

	// If there's no actor at the target data, end the ability: Invalid Target
	if (!IsValid(TargetHit->GetActor()))
	{
		CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
		RemoveCooldownEffect(GetCurrentActorInfo()->AbilitySystemComponent.Get());
		return;
	}

	// Create the telekinesis movement task:
	// This task will perform the physical grabbing movement on target
	AbilityTask =
		UPETelekinesisAbility_Task::PETelekinesisAbilityMovement(this, FName("TelekinesisTask"),
		                                                         ThrowIntensity, TargetHit->GetActor());

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
	// Play a animation montage
	ActivateWaitMontageTask();
}

void UPETelekinesisAbility::WaitGameplayEvent_Callback_Implementation(FGameplayEventData Payload)
{
	UGameplayStatics::SpawnSoundAttached(ImpulseSound, AbilityTask->GetTelekinesisTarget()->GetRootComponent());

	// When the AnimNotify is triggered, will launch the grabbed actor
	// in the direction of the camera and end the ability
	AbilityTask->ThrowObject();
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}
