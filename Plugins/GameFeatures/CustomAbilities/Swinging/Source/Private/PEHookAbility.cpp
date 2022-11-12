// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "PEHookAbility.h"
#include "PEHookAbility_Task.h"
#include "GameFramework/Character.h"
#include "GAS/Targeting/PELineTargeting.h"
#include "GAS/System/PETrace.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Management/Data/PEGlobalTags.h"

UPEHookAbility::UPEHookAbility(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), HookIntensity(3000.f), MaxHookIntensity(0.f)
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.Swinging"));

	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(GlobalTag_RegenBlock_Stamina));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(GlobalTag_RegenBlock_Mana));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(GlobalTag_AimingBlockedState));

	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(GlobalTag_WeaponSlot_Base));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(GlobalTag_AimingState));

	bWaitCancel = false;
	bIgnoreCooldown = true;
}

void UPEHookAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	bIgnoreCooldown = true;
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// Activate tasks: Animation Montage and Wait for GameplayEvent (Anim Notify)
	ActivateWaitMontageTask(NAME_None, 1.5f);
	ActivateWaitGameplayEventTask(FGameplayTag::RequestGameplayTag("Data.Notify.Ability"));
}

void UPEHookAbility::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	// Ability will end when the input is released: Release the hook
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UPEHookAbility::WaitGameplayEvent_Callback_Implementation(FGameplayEventData Payload)
{
	// Will start a targeting task when the animation notify is triggered (will try to start the hook)
	FPETargetActorSpawnParams TargetingParams;
	TargetingParams.StartLocation = MakeTargetLocationInfoFromOwnerSkeletalMeshComponent("hand_l");

	ActivateWaitTargetDataTask(EGameplayTargetingConfirmation::Instant, APELineTargeting::StaticClass(), TargetingParams);
}

void UPEHookAbility::WaitTargetData_Callback_Implementation(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	// If target is invalid, end the ability
	if (!TargetDataHandle.IsValid(0))
	{
		CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
		return;
	}

	// Get the first target only since this is a single target ability
	const FGameplayAbilityTargetData* const TargetData = TargetDataHandle.Get(0);
	const FHitResult* const TargetHit = TargetData->GetHitResult();

	// If there's no actor at the target data, end the ability: Invalid Target
	if (!IsValid(TargetHit->GetActor()))
	{
		CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
		return;
	}

	// Create and initialize the hook movement task: This task will perform the physical hook movement
	TaskHandle = UPEHookAbility_Task::HookAbilityMovement(this, TEXT("HookTask"), *TargetHit, HookIntensity, MaxHookIntensity);
	TaskHandle->ReadyForActivation();

	FGameplayCueParameters Params;
	Params.Location = TargetHit->Location;
	Params.TargetAttachComponent = TargetHit->GetComponent();

	TargetData->AddTargetDataToGameplayCueParameters(Params);

	ActivateGameplayCues(FGameplayTag::RequestGameplayTag("GameplayCue.Swinging"), Params);

	// If the target is a character, will finish this ability after AbilityActiveTime seconds
	if (TargetHit->GetActor()->GetClass()->IsChildOf<ACharacter>() && TargetHit->GetActor() != GetAvatarActorFromActorInfo() || TargetHit->GetComponent()->GetClass()->IsChildOf<UGeometryCollectionComponent>())
	{
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda([=]() -> void
		{
			if (IsValid(this) && IsActive())
			{
				EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
			}
		});

		GetWorld()->GetTimerManager().SetTimer(CancelationTimerHandle, TimerDelegate, AbilityActiveTime, false);
	}

	// Start waiting for confirm input
	ActivateWaitConfirmInputTask();
}

void UPEHookAbility::WaitConfirmInput_Callback_Implementation()
{
	// If the confirm input is pressed, will add a impulse to ability owner
	// and to the target/grabbed actor, if simulates physics

	if (ACharacter* const Player = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		PlayAbilitySoundAttached(Player->GetMesh());

		const FVector ImpulseVector = (TaskHandle->GetLastHookLocation() - Player->GetActorLocation()).GetSafeNormal() * HookIntensity;

		Player->LaunchCharacter(ImpulseVector, false, true);

		if (TaskHandle->GetHitResult().GetComponent()->IsSimulatingPhysics())
		{
			TaskHandle->GetHitResult().GetComponent()->AddImpulse(-1.f * ImpulseVector);
		}
		else if (ACharacter* const TargetPlayer = Cast<ACharacter>(TaskHandle->GetHitResult().GetActor()))
		{
			TargetPlayer->LaunchCharacter(-1.f * ImpulseVector, false, true);
		}

		// This ability will commit cooldown after this point
		bIgnoreCooldown = false;
		CommitAbilityCooldown(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
	}

	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}
