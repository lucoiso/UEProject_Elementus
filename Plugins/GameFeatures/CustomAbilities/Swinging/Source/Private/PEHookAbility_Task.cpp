// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "PEHookAbility_Task.h"
#include <GameFramework/Character.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <GeometryCollection/GeometryCollectionComponent.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(PEHookAbility_Task)

UPEHookAbility_Task::UPEHookAbility_Task(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bTickingTask = false;
	bIsFinished = false;
}

UPEHookAbility_Task* UPEHookAbility_Task::HookAbilityMovement(UGameplayAbility* OwningAbility, const FName TaskInstanceName, const FHitResult HitResult, const float HookIntensity, const float HookMaxForce)
{
	UPEHookAbility_Task* const MyObj = NewAbilityTask<UPEHookAbility_Task>(OwningAbility, TaskInstanceName);
	MyObj->Intensity = HookIntensity;
	MyObj->HitDataHandle = HitResult;
	MyObj->MaxForce = HookMaxForce;

	return MyObj;
}

void UPEHookAbility_Task::Activate()
{
	Super::Activate();
	check(Ability);

	HookOwner = Cast<ACharacter>(GetAvatarActor());

	if (ensureAlwaysMsgf(HookOwner.IsValid(), TEXT("%s - Task %s failed to activate because have a invalid owner"), *FString(__func__), *GetName()))
	{
		CurrentHookLocation = HitDataHandle.Location;

		HitTarget = Cast<ACharacter>(HitDataHandle.GetActor());
		if (!HitTarget.IsValid())
		{
			HitTarget.Reset();
		}

		if (IsValid(HitDataHandle.GetComponent()))
		{
			if (HitDataHandle.GetComponent()->IsSimulatingPhysics())
			{
				HitDataHandle.GetComponent()->WakeAllRigidBodies();
			}

			if (ShouldBroadcastAbilityTaskDelegates())
			{
				OnHooking.ExecuteIfBound(true);
			}

			bTickingTask = true;
			return;
		}
	}

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnHooking.ExecuteIfBound(false);
	}

	bIsFinished = true;
	EndTask();
}

FVector UPEHookAbility_Task::GetLastHookLocation() const
{
	return CurrentHookLocation;
}

FHitResult UPEHookAbility_Task::GetHitResult() const
{
	return HitDataHandle;
}

void UPEHookAbility_Task::TickTask(const float DeltaTime)
{
	if (bIsFinished)
	{
		EndTask();
		return;
	}

	Super::TickTask(DeltaTime);

	if (IsValid(HitDataHandle.GetComponent()))
	{
		const bool bIsTargetMovable = HitDataHandle.GetComponent()->Mobility == EComponentMobility::Movable;

		// UGeometryCollectionComponent is a special case, it is movable but
		// we can't get individual geometry bones via targeting (HitDataHandle.BoneName is returning None)
		// To avoid wrong location, we will use the final location of the hook instead of the hit location
		CurrentHookLocation = bIsTargetMovable && !HitDataHandle.GetComponent()->GetClass()->IsChildOf<UGeometryCollectionComponent>() ? HitDataHandle.GetComponent()->GetSocketLocation(HitDataHandle.BoneName) : HitDataHandle.Location;

		if (const FVector Difference = CurrentHookLocation - HookOwner->GetActorLocation();
			Difference.Size() >= 100.f)
		{
			const FVector BaseForce = Difference * Intensity;
			const FVector HookForce = MaxForce > 0.f ? BaseForce.GetClampedToMaxSize(MaxForce) : BaseForce;

			HookOwner->GetCharacterMovement()->AddForce(HookForce);

			if (HitTarget.IsValid())
			{
				HitTarget->GetCharacterMovement()->AddForce(-1.f * HookForce);
			}
			else if (bIsTargetMovable && HitDataHandle.GetComponent()->IsSimulatingPhysics())
			{
				HitDataHandle.GetComponent()->AddForce(-1.f * HookForce);
			}
		}
	}
	else
	{
		bIsFinished = true;
		EndTask();
	}
}

void UPEHookAbility_Task::OnDestroy(const bool AbilityIsEnding)
{
	UE_LOG(LogGameplayTasks, Display, TEXT("%s - Task %s ended"), *FString(__func__), *GetName());

	bIsFinished = true;

	HitTarget.Reset();
	HookOwner.Reset();

	Super::OnDestroy(AbilityIsEnding);
}
