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
		if (Ability->GetActorInfo().IsNetAuthority())
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
	}

	bIsFinished = true;
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

	const float Multiplier = (2.f + DeltaTime);

	if (IsValid(HitDataHandle.GetActor()) &&
		HitDataHandle.GetActor()->IsRootComponentMovable() &&
		HitDataHandle.GetActor()->GetRootComponent()->IsSimulatingPhysics())
	{
		HookLocation = HitDataHandle.GetActor()->GetActorLocation();

		if (!HitDataHandle.GetActor()->GetClass()->IsChildOf<APECharacterBase>())
		{
			HitDataHandle.GetComponent()->AddImpulse(
				Multiplier * (HookOwner->GetActorLocation() - HitDataHandle.GetActor()->GetActorLocation()));
		}
	}

	const FVector Dif = HookLocation - HookOwner->GetActorLocation();

	if (Dif.Size() <= 250.f)
	{
		const float Dot = FVector::DotProduct(Dif, HookOwner->GetVelocity());
		const FVector Force = Dif.GetSafeNormal() * Dot * (Multiplier * -1.f);

		HookOwner->GetCharacterMovement()->AddForce(Force);
	}

	else
	{
		const FVector HookVelocity = FVector(0.75f, 0.75f, 1.f) * Dif.GetClampedToMaxSize(17.5f) * Multiplier;

		if (HitTarget.IsValid())
		{
			HitTarget->GetCharacterMovement()->AddImpulse(-1.f * HookVelocity, true);
		}

		HookOwner->GetCharacterMovement()->AddImpulse(HookVelocity, true);
	}
}

void UHookAbility_Task::OnDestroy(const bool AbilityIsEnding)
{
	UE_LOG(LogGameplayTasks, Warning, TEXT("Task %s ended"), *GetName());
	
	bIsFinished = true;

	HitTarget.Reset();
	HookOwner.Reset();

	Super::OnDestroy(AbilityIsEnding);
}