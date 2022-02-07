#include "HookAbility_Task.h"
#include "Actors/Character/PECharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UHookAbility_Task::UHookAbility_Task(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
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
}

void UHookAbility_Task::TickTask(const float DeltaTime)
{
	if (bIsFinished)
	{
		return;
	}

	Super::TickTask(DeltaTime);

	FVector HookLocation = HitDataHandle.Location;

	APECharacterBase* HookOwner = Cast<APECharacterBase>(GetAvatarActor());

	if (IsValid(HookOwner))
	{
		if (HitDataHandle.GetActor()->IsRootComponentMovable() && HitDataHandle.GetActor()->GetRootComponent()->
			IsSimulatingPhysics())
		{
			HookLocation = HitDataHandle.GetActor()->GetActorLocation();

			if (!Cast<APECharacterBase>(HitDataHandle.GetActor()))
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

			if (Cast<APECharacterBase>(HitDataHandle.GetActor()))
			{
				Cast<APECharacterBase>(HitDataHandle.GetActor())->LaunchCharacter(-1.f * HookVelocity, false, false);
			}

			HookOwner->LaunchCharacter(HookVelocity, false, false);
		}
	}
	else
	{
		UE_LOG(LogGameplayTasks, Warning, TEXT("Task %s ended"), *GetName());

		bIsFinished = true;
		EndTask();
	}
}

void UHookAbility_Task::OnDestroy(const bool AbilityIsEnding)
{
	bIsFinished = true;
	Super::OnDestroy(AbilityIsEnding);
}
