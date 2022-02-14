// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "GAS/System/GASGameplayAbility.h"
#include "GAS/System/GASAbilitySystemComponent.h"
#include "GAS/Tasks/SpawnProjectile_Task.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitConfirm.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayTag.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_SpawnActor.h"

#include "Abilities/GameplayAbilityTargetActor_SingleLineTrace.h"
#include "Abilities/GameplayAbilityTargetActor_GroundTrace.h"

#include "Actors/Character/PECharacterBase.h"
#include "Actors/World/ProjectileActor.h"

#include "Camera/CameraComponent.h"

#include "GameplayEffect.h"

UGASGameplayAbility::UGASGameplayAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	  AbilityActiveTime(0),
	  AbilityMaxRange(0),
	  bEndAbilityAfterActiveTime(false),
	  bAutoActivateOnGrant(false)
{
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag("State.Dead"));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag("State.Stunned"));

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	bIsCancelable = true;
}

void UGASGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	ABILITY_VLOG(this, Warning, TEXT("Ability %s given to %s."), *GetName(), *ActorInfo->AvatarActor->GetName());

	Super::OnGiveAbility(ActorInfo, Spec);

	if (!Spec.IsActive() && bAutoActivateOnGrant)
	{
		ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
	}
}

void UGASGameplayAbility::PreActivate(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo,
                                      const FGameplayAbilityActivationInfo ActivationInfo,
                                      FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
                                      const FGameplayEventData* TriggerEventData)
{
	ABILITY_VLOG(this, Warning, TEXT("Trying pre-activate %s ability."), *GetName());

	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);

	if (!CommitCheck(Handle, ActorInfo, ActivationInfo) || !HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		ABILITY_VLOG(this, Warning, TEXT("%s is being cancelled before activation."), *GetName());
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}

	ActivationBlockedTags.AppendTags(AbilityTags);

	if (IsInstantiated())
	{
		UAbilityTask_WaitGameplayTagAdded* WaitDeadTagAddedTask =
			UAbilityTask_WaitGameplayTagAdded::WaitGameplayTagAdd(this, FGameplayTag::RequestGameplayTag("State.Dead"));

		WaitDeadTagAddedTask->Added.AddDynamic(this, &UGASGameplayAbility::K2_EndAbility);
		WaitDeadTagAddedTask->ReadyForActivation();

		UAbilityTask_WaitGameplayTagAdded* WaitStunTagAddedTask =
			UAbilityTask_WaitGameplayTagAdded::WaitGameplayTagAdd(
				this, FGameplayTag::RequestGameplayTag("State.Stunned"));

		WaitStunTagAddedTask->Added.AddDynamic(this, &UGASGameplayAbility::K2_EndAbility);
		WaitStunTagAddedTask->ReadyForActivation();

		if (CanBeCanceled())
		{
			ActivateWaitCancelInputTask();
		}
	}
}

void UGASGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo,
                                          const FGameplayAbilityActivationInfo ActivationInfo,
                                          const FGameplayEventData* TriggerEventData)
{
	ABILITY_VLOG(this, Warning, TEXT("%s ability successfully activated."), *GetName());

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (bEndAbilityAfterActiveTime)
	{
		FTimerDelegate TimerDelegate;
		FTimerHandle TimerHandle;
		TimerDelegate.BindLambda([this, Handle, ActorInfo, ActivationInfo]() -> void
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		});

		ActorInfo->AvatarActor->GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, AbilityActiveTime,
		                                                               false);
	}
}

void UGASGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo,
                                     const FGameplayAbilityActivationInfo ActivationInfo,
                                     const bool bReplicateEndAbility,
                                     const bool bWasCancelled)
{
	ABILITY_VLOG(this, Warning, TEXT("Ending %s ability."), *GetName());

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (IsValid(GetCostGameplayEffect()) && GetCostGameplayEffect()->DurationPolicy ==
		EGameplayEffectDurationType::Infinite)
	{
		ActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(
			GetCostGameplayEffect()->GetClass(), ActorInfo->AbilitySystemComponent.Get());
	}

	if (AbilityActiveTime <= 0)
	{
		for (const FGameplayEffectGroupedData& EffectGroup : SelfAbilityEffects)
		{
			ActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(
				EffectGroup.EffectClass, ActorInfo->AbilitySystemComponent.Get());
		}
	}
}

void UGASGameplayAbility::ActivateGameplayCues(const FGameplayTag GameplayCueTag,
                                               const FGameplayCueParameters Parameters,
                                               UAbilitySystemComponent* SourceAbilitySystem)
{
	ABILITY_VLOG(this, Warning, TEXT("Activating %s ability associated Gameplay Cues."), *GetName());

	if (GameplayCueTag.IsValid())
	{
		SourceAbilitySystem->AddGameplayCue(GameplayCueTag, Parameters);
		TrackedGameplayCues.Add(GameplayCueTag);
	}
}

void UGASGameplayAbility::BP_ApplyAbilityEffectsToSelf()
{
	check(CurrentActorInfo);

	ApplyAbilityEffectsToSelf(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);
}

void UGASGameplayAbility::ApplyAbilityEffectsToSelf(const FGameplayAbilitySpecHandle Handle,
                                                    const FGameplayAbilityActorInfo* ActorInfo,
                                                    const FGameplayAbilityActivationInfo ActivationInfo)
{
	ABILITY_VLOG(this, Warning, TEXT("Applying %s ability effects to owner."), *GetName());

	for (const FGameplayEffectGroupedData& EffectGroup : SelfAbilityEffects)
	{
		const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(
			Handle, ActorInfo, ActivationInfo, EffectGroup.EffectClass);

		for (const TPair<FGameplayTag, float>& StackedData : EffectGroup.SetByCallerStackedData)
		{
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(StackedData.Key,
			                                               StackedData.Value);
		}

		if (SpecHandle.IsValid())
		{
			ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
		}
	}
}

void UGASGameplayAbility::BP_RemoveAbilityEffectsFromSelf()
{
	check(CurrentActorInfo);

	RemoveAbilityEffectsFromSelf(CurrentActorInfo);
}

void UGASGameplayAbility::RemoveAbilityEffectsFromSelf(const FGameplayAbilityActorInfo* ActorInfo)
{
	ABILITY_VLOG(this, Warning, TEXT("Removing %s ability effects from owner."), *GetName());

	for (const FGameplayEffectGroupedData& EffectGroup : SelfAbilityEffects)
	{
		FGameplayEffectQuery Query;
		Query.EffectDefinition = EffectGroup.EffectClass;

		ActorInfo->AbilitySystemComponent.Get()->RemoveActiveEffects(Query);
	}
}

void UGASGameplayAbility::BP_ApplyAbilityEffectsToTarget(const FGameplayAbilityTargetDataHandle TargetDataHandle)
{
	check(CurrentActorInfo);

	ApplyAbilityEffectsToTarget(TargetDataHandle, CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);
}

void UGASGameplayAbility::ApplyAbilityEffectsToTarget(const FGameplayAbilityTargetDataHandle TargetDataHandle,
                                                      const FGameplayAbilitySpecHandle Handle,
                                                      const FGameplayAbilityActorInfo* ActorInfo,
                                                      const FGameplayAbilityActivationInfo ActivationInfo)
{
	ABILITY_VLOG(this, Warning, TEXT("Applying %s ability effects to targets."), *GetName());

	for (const FGameplayEffectGroupedData& EffectGroup : TargetAbilityEffects)
	{
		const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(
			Handle, ActorInfo, ActivationInfo, EffectGroup.EffectClass);

		for (const TPair<FGameplayTag, float>& StackedData : EffectGroup.SetByCallerStackedData)
		{
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(StackedData.Key,
			                                               StackedData.Value);
		}

		if (SpecHandle.IsValid())
		{
			ApplyGameplayEffectSpecToTarget(Handle, ActorInfo, ActivationInfo, SpecHandle, TargetDataHandle);
		}
	}
}

void UGASGameplayAbility::BP_SpawnProjectileWithTargetEffects(
	const TSubclassOf<AProjectileActor> ProjectileClass,
	const FTransform ProjectileTransform, const FVector ProjectileFireDirection)
{
	check(CurrentActorInfo);

	SpawnProjectileWithTargetEffects(ProjectileClass, ProjectileTransform, ProjectileFireDirection, CurrentSpecHandle,
	                                 CurrentActorInfo, CurrentActivationInfo);
}

void UGASGameplayAbility::SpawnProjectileWithTargetEffects(const TSubclassOf<AProjectileActor> ProjectileClass,
                                                           const FTransform ProjectileTransform,
                                                           const FVector ProjectileFireDirection,
                                                           const FGameplayAbilitySpecHandle Handle,
                                                           const FGameplayAbilityActorInfo* ActorInfo,
                                                           const FGameplayAbilityActivationInfo ActivationInfo)
{
	ABILITY_VLOG(this, Warning, TEXT("Spawning %s ability projectile."), *GetName());

	TArray<FGameplayEffectSpecHandle> EffectSpecs;

	for (const FGameplayEffectGroupedData& EffectGroup : TargetAbilityEffects)
	{
		const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(
			Handle, ActorInfo, ActivationInfo, EffectGroup.EffectClass);

		for (const TPair<FGameplayTag, float>& StackedData : EffectGroup.SetByCallerStackedData)
		{
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(StackedData.Key,
			                                               StackedData.Value);
		}

		EffectSpecs.Add(SpecHandle);
	}

	USpawnProjectile_Task* SpawnProjectile_Task =
		USpawnProjectile_Task::SpawnProjectile(this, ProjectileClass, ProjectileTransform, ProjectileFireDirection,
		                                       EffectSpecs);

	SpawnProjectile_Task->ReadyForActivation();
}

void UGASGameplayAbility::RemoveCooldownEffect(UAbilitySystemComponent* SourceAbilitySystem) const
{
	if (IsValid(GetCooldownGameplayEffect()))
	{
		ABILITY_VLOG(this, Warning, TEXT("Removing %s ability cooldown."), *GetName());

		FGameplayTagContainer CooldownEffectTags;
		GetCooldownGameplayEffect()->GetOwnedGameplayTags(CooldownEffectTags);
		SourceAbilitySystem->RemoveActiveEffectsWithAppliedTags(CooldownEffectTags);
	}
}

void UGASGameplayAbility::DoAbilityLineTrace(FGASTraceDataHandle& TraceDataHandle, AActor* OwnerActor) const
{
	ABILITY_VLOG(this, Warning, TEXT("Performing %s ability line trace."), *GetName());

	TraceDataHandle.Hit = FHitResult();
	TraceDataHandle.MaxRange = AbilityMaxRange;
	TraceDataHandle.QueryParams.AddIgnoredActor(OwnerActor);

	TraceDataHandle.FilterHandle.Filter->SelfActor = OwnerActor;

	USceneComponent* CameraComponent =
		Cast<USceneComponent>(Cast<APECharacterBase>(OwnerActor)->GetFollowCamera());

	if (!TraceDataHandle.Component.IsValid() && IsValid(CameraComponent))
	{
		TraceDataHandle.Component = CameraComponent;
	}

	const FVector Start = TraceDataHandle.Component->GetComponentLocation();
	const FVector End = Start + (TraceDataHandle.Component->GetForwardVector() * AbilityMaxRange);

#ifdef UE_BUILD_DEBUG
	const FName TraceTag("LineTraceDebugTag");
	TraceDataHandle.Component->GetWorld()->DebugDrawTraceTag = TraceTag;
	TraceDataHandle.QueryParams.TraceTag = TraceTag;
#endif // !UE_BUILD_DEBUG

	AGameplayAbilityTargetActor_SingleLineTrace::LineTraceWithFilter(TraceDataHandle.Hit,
	                                                                 TraceDataHandle.Component->GetWorld(),
	                                                                 TraceDataHandle.FilterHandle,
	                                                                 Start,
	                                                                 End,
	                                                                 TraceDataHandle.CollisionProfile,
	                                                                 TraceDataHandle.QueryParams);
}

FGameplayAbilityTargetDataHandle UGASGameplayAbility::MakeTargetDataHandleFromSingleHitResult(
	const FHitResult HitResult)
{
	FGameplayAbilityTargetDataHandle TargetData;

	FGameplayAbilityTargetData_SingleTargetHit* NewData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
	TargetData.Add(NewData);

	return TargetData;
}

FGameplayAbilityTargetDataHandle UGASGameplayAbility::MakeTargetDataHandleFromMultipleHitResults(
	const TArray<FHitResult> HitResults)
{
	FGameplayAbilityTargetDataHandle TargetData;

	for (const FHitResult& HitResult : HitResults)
	{
		FGameplayAbilityTargetData_SingleTargetHit* NewData = new
			FGameplayAbilityTargetData_SingleTargetHit(HitResult);
		TargetData.Add(NewData);
	}

	return TargetData;
}

FGameplayAbilityTargetDataHandle UGASGameplayAbility::MakeTargetDataHandleFromActorArray(
	const TArray<AActor*> TargetActors)
{
	if (TargetActors.Num() > 0)
	{
		FGameplayAbilityTargetData_ActorArray* NewData = new FGameplayAbilityTargetData_ActorArray();
		NewData->TargetActorArray.Append(TargetActors);
		return FGameplayAbilityTargetDataHandle(NewData);
	}

	return FGameplayAbilityTargetDataHandle();
}

FGameplayAbilityTargetDataHandle UGASGameplayAbility::MakeTargetDataHandleFromTransform(const FTransform TransformData)
{
	return FGameplayAbilityTargetDataHandle();
}

void UGASGameplayAbility::ActivateWaitMontageTask()
{
	UAbilityTask_PlayMontageAndWait* AbilityTask_PlayMontageAndWait =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, "WaitMontageTask", AbilityAnimation);

	AbilityTask_PlayMontageAndWait->OnBlendOut.AddDynamic(this, &UGASGameplayAbility::WaitMontage_Callback);
	AbilityTask_PlayMontageAndWait->OnInterrupted.AddDynamic(this, &UGASGameplayAbility::K2_EndAbility);
	AbilityTask_PlayMontageAndWait->OnCancelled.AddDynamic(this, &UGASGameplayAbility::K2_CancelAbility);

	AbilityTask_PlayMontageAndWait->ReadyForActivation();
}

void UGASGameplayAbility::ActivateWaitTargetDataTask(
	const TEnumAsByte<EGameplayTargetingConfirmation::Type> TargetingConfirmation,
	const TSubclassOf<AGameplayAbilityTargetActor> TargetActorClass)
{
	UAbilityTask_WaitTargetData* AbilityTask_WaitTargetData =
		UAbilityTask_WaitTargetData::WaitTargetData(this, "WaitTargetDataTask", TargetingConfirmation,
		                                            TargetActorClass);

	AbilityTask_WaitTargetData->Cancelled.AddDynamic(this, &UGASGameplayAbility::WaitTargetData_Callback);
	AbilityTask_WaitTargetData->ValidData.AddDynamic(this, &UGASGameplayAbility::WaitTargetData_Callback);

	AbilityTask_WaitTargetData->ReadyForActivation();
}

void UGASGameplayAbility::ActivateWaitConfirmInputTask()
{
	UAbilityTask_WaitConfirm* AbilityTask_WaitConfirm =
		UAbilityTask_WaitConfirm::WaitConfirm(this);

	AbilityTask_WaitConfirm->OnConfirm.AddDynamic(this, &UGASGameplayAbility::WaitConfirmInput_Callback);

	AbilityTask_WaitConfirm->ReadyForActivation();
}

void UGASGameplayAbility::ActivateWaitCancelInputTask()
{
	UAbilityTask_WaitCancel* AbilityTask_WaitCancel =
		UAbilityTask_WaitCancel::WaitCancel(this);

	AbilityTask_WaitCancel->OnCancel.AddDynamic(this, &UGASGameplayAbility::WaitCancelInput_Callback);

	AbilityTask_WaitCancel->ReadyForActivation();
}

void UGASGameplayAbility::ActivateWaitAddedTagTask(const FGameplayTag Tag)
{
	UAbilityTask_WaitGameplayTagAdded* AbilityTask_WaitGameplayTagAdded =
		UAbilityTask_WaitGameplayTagAdded::WaitGameplayTagAdd(this, Tag);

	AbilityTask_WaitGameplayTagAdded->Added.AddDynamic(this, &UGASGameplayAbility::WaitAddedTag_Callback);

	AbilityTask_WaitGameplayTagAdded->ReadyForActivation();
}

void UGASGameplayAbility::ActivateWaitRemovedTagTask(const FGameplayTag Tag)
{
	UAbilityTask_WaitGameplayTagRemoved* AbilityTask_WaitGameplayTagRemoved =
		UAbilityTask_WaitGameplayTagRemoved::WaitGameplayTagRemove(this, Tag);

	AbilityTask_WaitGameplayTagRemoved->Removed.AddDynamic(this, &UGASGameplayAbility::WaitRemovedTag_Callback);

	AbilityTask_WaitGameplayTagRemoved->ReadyForActivation();
}

void UGASGameplayAbility::ActivateWaitGameplayEventTask(const FGameplayTag EventTag)
{
	UAbilityTask_WaitGameplayEvent* AbilityTask_WaitGameplayEvent =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, EventTag);

	AbilityTask_WaitGameplayEvent->EventReceived.AddDynamic(this, &UGASGameplayAbility::WaitGameplayEvent_Callback);

	AbilityTask_WaitGameplayEvent->ReadyForActivation();
}

void UGASGameplayAbility::ActivateSpawnActorTask(const FGameplayAbilityTargetDataHandle TargetDataHandle,
                                                 const TSubclassOf<AActor> ActorClass)
{
	UAbilityTask_SpawnActor* AbilityTask_SpawnActor =
		UAbilityTask_SpawnActor::SpawnActor(this, TargetDataHandle, ActorClass);

	AbilityTask_SpawnActor->DidNotSpawn.AddDynamic(this, &UGASGameplayAbility::SpawnActor_Callback);
	AbilityTask_SpawnActor->Success.AddDynamic(this, &UGASGameplayAbility::SpawnActor_Callback);

	AbilityTask_SpawnActor->ReadyForActivation();
}

void UGASGameplayAbility::WaitCancelInput_Callback()
{
	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}
