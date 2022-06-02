// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "PEHookAbility.h"
#include "PEHookAbility_Task.h"
#include "Actors/Character/PECharacter.h"
#include "GAS/Targeting/PELineTargeting.h"

UPEHookAbility::UPEHookAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	  HookIntensity(3000.f)
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.Swinging"));

	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayEffect.Debuff.Regeneration.Block.Stamina"));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayEffect.Debuff.Regeneration.Block.Mana"));

	bWaitCancel = false;
	bIgnoreCooldown = true;
}

void UPEHookAbility::ActivateAbility
(const FGameplayAbilitySpecHandle Handle,
 const FGameplayAbilityActorInfo* ActorInfo,
 const FGameplayAbilityActivationInfo ActivationInfo,
 const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	bIgnoreCooldown = true;

	ActivateWaitMontageTask(NAME_None, 1.5f);
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

	ActivateWaitTargetDataTask(EGameplayTargetingConfirmation::Instant,
	                           APELineTargeting::StaticClass(), TargetingParams);
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

	TaskHandle = UPEHookAbility_Task::HookAbilityMovement(this, FName("HookTask"), *TargetHit, HookIntensity);

	TaskHandle->ReadyForActivation();

	FGameplayCueParameters Params;
	Params.Location = TargetHit->Location;
	Params.TargetAttachComponent = TargetHit->GetComponent();

	TargetData->AddTargetDataToGameplayCueParameters(Params);

	ActivateGameplayCues(FGameplayTag::RequestGameplayTag("GameplayCue.Swinging"), Params,
	                     GetCurrentActorInfo()->AbilitySystemComponent.Get());

	if (Cast<APECharacter>(TargetHit->GetActor()) && TargetHit->GetActor() != GetAvatarActorFromActorInfo())
	{
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda([=]() -> void
		{
			if (IsActive())
			{
				EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true,
				           false);
			}
		});

		GetWorld()->GetTimerManager().SetTimer(CancelationTimerHandle, TimerDelegate, AbilityActiveTime, false);
	}

	ActivateWaitConfirmInputTask();
}

void UPEHookAbility::WaitConfirmInput_Callback_Implementation()
{
	if (APECharacter* Player = Cast<APECharacter>(GetAvatarActorFromActorInfo()))
	{
		const FVector ImpulseVector = (TaskHandle->GetLastHookLocation() - Player->GetActorLocation()).GetSafeNormal() *
			HookIntensity;

		Player->LaunchCharacter(ImpulseVector, false, true);

		if (TaskHandle->GetHitResult().GetComponent()->IsSimulatingPhysics())
		{
			TaskHandle->GetHitResult().GetComponent()->AddImpulse(-1.f * ImpulseVector);
		}
		else if (APECharacter* TargetPlayer = Cast<APECharacter>(TaskHandle->GetHitResult().GetActor()))
		{
			TargetPlayer->LaunchCharacter(-1.f * ImpulseVector, false, true);
		}

		bIgnoreCooldown = false;
		CommitAbilityCooldown(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
	}

	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}
