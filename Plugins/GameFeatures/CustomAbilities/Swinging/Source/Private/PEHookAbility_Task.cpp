// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "PEHookAbility_Task.h"
#include "Actors/Character/PECharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UPEHookAbility_Task::UPEHookAbility_Task(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = false;
	bIsFinished = false;
}

UPEHookAbility_Task* UPEHookAbility_Task::HookAbilityMovement(UGameplayAbility* OwningAbility,
	const FName TaskInstanceName,
	const FHitResult HitResult)
{
	UPEHookAbility_Task* MyObj = NewAbilityTask<UPEHookAbility_Task>(OwningAbility, TaskInstanceName);

	MyObj->HitDataHandle = HitResult;

	return MyObj;
}

void UPEHookAbility_Task::Activate()
{
	Super::Activate();

	if (ensureMsgf(IsValid(Ability), TEXT("%s have a invalid Ability"), *GetName()))
	{
		HookOwner = Cast<APECharacterBase>(Ability->GetAvatarActorFromActorInfo());

		if (ensureMsgf(HookOwner.IsValid(), TEXT("%s have a invalid Owner"), *GetName()))
		{
			HitTarget = Cast<APECharacterBase>(HitDataHandle.GetActor());
			if (!HitTarget.IsValid())
			{
				HitTarget.Reset();
			}

			if (IsValid(HitDataHandle.GetActor()))
			{
				if (IsValid(HitDataHandle.GetComponent()) && HitDataHandle.GetComponent()->IsSimulatingPhysics())
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
	}

	bIsFinished = true;
	EndTask();
}

void UPEHookAbility_Task::TickTask(const float DeltaTime)
{
	if (bIsFinished)
	{
		EndTask();
		return;
	}

	Super::TickTask(DeltaTime);

	if (IsValid(HitDataHandle.GetActor()))
	{
		const bool bIsTargetMovableAndSimulatingPhysics =
			HitDataHandle.GetActor()->IsRootComponentMovable() &&
			HitDataHandle.GetActor()->GetRootComponent()->IsSimulatingPhysics();

		const FVector HookLocationToUse =
			bIsTargetMovableAndSimulatingPhysics
			? HitDataHandle.GetActor()->GetActorLocation()
			: HitDataHandle.Location;

		const FVector Difference = HookLocationToUse - HookOwner->GetActorLocation();
		if (Difference.Size() >= 500.f)
		{
			const FVector HookForce = Difference * (0.75f / DeltaTime);

			HookOwner->GetCharacterMovement()->AddForce(HookForce);

			if (bIsTargetMovableAndSimulatingPhysics && !HitDataHandle.GetActor()->GetClass()->IsChildOf<
				APECharacterBase>())
			{
				HitDataHandle.GetComponent()->AddForce(-1.f * HookForce);
			}

			else if (HitTarget.IsValid())
			{
				HitTarget->GetCharacterMovement()->AddForce(-1.f * HookForce);
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
	UE_LOG(LogGameplayTasks, Warning, TEXT("Task %s ended"), *GetName());

	bIsFinished = true;

	HitTarget.Reset();
	HookOwner.Reset();

	Super::OnDestroy(AbilityIsEnding);
}