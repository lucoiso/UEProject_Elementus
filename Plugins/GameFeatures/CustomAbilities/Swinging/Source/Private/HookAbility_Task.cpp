// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "HookAbility_Task.h"
#include "Actors/Character/PECharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UHookAbility_Task::UHookAbility_Task(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = false;
	bIsFinished = false;
}

UHookAbility_Task* UHookAbility_Task::HookAbilityMovement(UGameplayAbility* OwningAbility, const FName TaskInstanceName,
	const FHitResult HitResult)
{
	UHookAbility_Task* MyObj = NewAbilityTask<UHookAbility_Task>(OwningAbility, TaskInstanceName);

	MyObj->HitDataHandle = HitResult;

	return MyObj;
}

void UHookAbility_Task::Activate()
{
	Super::Activate();

	if (ensureMsgf(IsValid(Ability), TEXT("%s have a invalid Ability"), *GetName()))
	{
		HookOwner = Cast<APECharacterBase>(GetAvatarActor());

		if (ensureMsgf(HookOwner.IsValid(), TEXT("%s have a invalid Owner"), *GetName()))
		{
			HitTarget = Cast<APECharacterBase>(HitDataHandle.GetActor());
			if (!HitTarget.IsValid())
			{
				HitTarget.Reset();
			}

			if (IsValid(HitDataHandle.GetActor()))
			{
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

	UE_LOG(LogGameplayTasks, Warning, TEXT("Task %s ended"), *GetName());
	EndTask();
}

void UHookAbility_Task::TickTask(const float DeltaTime)
{
	if (bIsFinished)
	{
		EndTask();
		return;
	}

	Super::TickTask(DeltaTime);

	FVector HookLocation = HitDataHandle.Location;

	if (IsValid(HitDataHandle.GetActor()) &&
		HitDataHandle.GetActor()->IsRootComponentMovable() &&
		HitDataHandle.GetActor()->GetRootComponent()->IsSimulatingPhysics())
	{
		HookLocation = HitDataHandle.GetActor()->GetActorLocation();

		if (!HitDataHandle.GetActor()->GetClass()->IsChildOf<APECharacterBase>())
		{
			HitDataHandle.GetComponent()->AddImpulse(
				2.5f * (HookOwner->GetActorLocation() - HitDataHandle.GetActor()->GetActorLocation()));
		}
	}

	FVector Dif = HookLocation - HookOwner->GetActorLocation();

	if (Dif.Size() <= 250.f)
	{
		Dif = HookLocation - HookOwner->GetActorLocation();
		const float Dot = Dif.DotProduct(Dif, HookOwner->GetVelocity());
		Dif.Normalize();
		const FVector Force = Dif * Dot * -2.5f;

		HookOwner->GetCharacterMovement()->AddForce(Force);
	}

	else
	{
		const FVector HookVelocity = FVector(0.5f, 0.5f, 1.f) * Dif.GetClampedToMaxSize(50.f);

		if (HitTarget.IsValid())
		{
			HitTarget->LaunchCharacter(-1.f * HookVelocity, false, false);
		}

		HookOwner->LaunchCharacter(HookVelocity, false, false);
	}
}

void UHookAbility_Task::OnDestroy(const bool AbilityIsEnding)
{
	bIsFinished = true;

	HitTarget.Reset();
	HookOwner.Reset();

	Super::OnDestroy(AbilityIsEnding);
}