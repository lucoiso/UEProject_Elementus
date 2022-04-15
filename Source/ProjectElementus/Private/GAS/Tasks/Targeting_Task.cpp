// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "GAS/Tasks/Targeting_Task.h"
#include "Abilities/GameplayAbilityTargetActor_GroundTrace.h"

UTargeting_Task::UTargeting_Task(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = false;
	bIsFinished = false;
}

UTargeting_Task* UTargeting_Task::StartTargetingAndWaitData(UGameplayAbility* OwningAbility, const FName TaskInstanceName,
	const TEnumAsByte<EGameplayTargetingConfirmation::Type> ConfirmationType,
	const TSubclassOf<AGameplayAbilityTargetActor_Trace> TargetActorClass,
	const FTargetActorSpawnParams Parameters)
{
	UTargeting_Task* MyObj = NewAbilityTask<UTargeting_Task>(OwningAbility, TaskInstanceName);
	MyObj->TargetActorClass = TargetActorClass;
	MyObj->ConfirmationType = ConfirmationType;
	MyObj->TargetingParams = Parameters;

	return MyObj;
}

void UTargeting_Task::Activate()
{
	Super::Activate();

	if (ensureMsgf(IsValid(Ability), TEXT("%s have a invalid Ability"), *GetName()))
	{
		if (TargetActorClass != nullptr)
		{
			TargetActor = GetWorld()->SpawnActorDeferred<AGameplayAbilityTargetActor>(TargetActorClass, FTransform::Identity);

			TargetActor->StartLocation = TargetingParams.StartLocation;
			TargetActor->ReticleClass = TargetingParams.ReticleClass;
			TargetActor->ReticleParams = TargetingParams.ReticleParams;
			TargetActor->bDebug = TargetingParams.bDebug;

			FGameplayTargetDataFilterHandle FilterHandle;
			FilterHandle.Filter = MakeShared<FGameplayTargetDataFilter>(TargetingParams.TargetFilter);
			TargetActor->Filter = FilterHandle;

			if (TargetActorClass.Get()->IsChildOf<AGameplayAbilityTargetActor_Trace>())
			{
				AGameplayAbilityTargetActor_Trace* TraceObj = Cast<AGameplayAbilityTargetActor_Trace>(TargetActor);

				TraceObj->MaxRange = TargetingParams.Range;
				TraceObj->bTraceAffectsAimPitch = TargetingParams.bTraceAffectsAimPitch;

				if (TargetActorClass.Get()->IsChildOf<AGameplayAbilityTargetActor_GroundTrace>())
				{
					AGameplayAbilityTargetActor_GroundTrace* GroundTraceObj = Cast<AGameplayAbilityTargetActor_GroundTrace>(TargetActor);

					GroundTraceObj->MaxRange = TargetingParams.Range;
					GroundTraceObj->bTraceAffectsAimPitch = TargetingParams.bTraceAffectsAimPitch;
					GroundTraceObj->CollisionRadius = TargetingParams.Radius;
					GroundTraceObj->CollisionHeight = TargetingParams.Height;
				}
			}

			TargetActor->FinishSpawning(FTransform::Identity);

			if (TargetActor.IsValid())
			{
				TargetActor->MasterPC = Ability->GetCurrentActorInfo()->PlayerController.Get();
				TargetActor->bDestroyOnConfirmation = TargetingParams.bDestroyOnConfirmation;
				TargetActor->ShouldProduceTargetDataOnServer = true;

				if (ShouldBroadcastAbilityTaskDelegates())
				{
					TargetActor->TargetDataReadyDelegate.AddUObject(this, &UTargeting_Task::OnTargetDataReadyCallback);
					TargetActor->CanceledDelegate.AddUObject(this, &UTargeting_Task::OnTargetDataCancelledCallback);
				}

				TargetActor->StartTargeting(Ability);

				if (TargetActor->ShouldProduceTargetData())
				{
					if (ConfirmationType == EGameplayTargetingConfirmation::Instant)
					{
						TargetActor->ConfirmTargeting();
					}
					else if (ConfirmationType == EGameplayTargetingConfirmation::UserConfirmed)
					{
						TargetActor->BindToConfirmCancelInputs();

						// Debugging
						bTickingTask = TargetingParams.bDebug;

						return;
					}
				}
			}
		}
	}

	bIsFinished = true;

	EndTask();
}

void UTargeting_Task::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& Data)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(Data);
	}
}

void UTargeting_Task::OnTargetDataCancelledCallback(const FGameplayAbilityTargetDataHandle& Data)
{
	bIsFinished = true;

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		Cancelled.Broadcast(Data);
	}

	EndTask();
}

void UTargeting_Task::OnDestroy(bool AbilityEnded)
{
	UE_LOG(LogGameplayTasks, Warning, TEXT("Task %s ended"), *GetName());

	bIsFinished = true;

	if (TargetActor.IsValid())
	{
		TargetActor->TargetDataReadyDelegate.RemoveAll(this);
		TargetActor->CanceledDelegate.RemoveAll(this);

		TargetActor->Destroy();
		TargetActor.Reset();
	}

	Super::OnDestroy(AbilityEnded);
}

void UTargeting_Task::TickTask(float DeltaTime)
{
	if (bIsFinished)
	{
		EndTask();
		return;
	}

	Super::TickTask(DeltaTime);

	// Debugging
	if (TargetActor.IsValid() && TargetingParams.bDebug)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.001f, FColor::Yellow, "ShouldProduceTargetData: " + FString::FromInt(TargetActor->ShouldProduceTargetData()));
		GEngine->AddOnScreenDebugMessage(-1, 0.001f, FColor::Yellow, "ShouldProduceTargetDataOnServer: " + FString::FromInt(TargetActor->ShouldProduceTargetDataOnServer));
		GEngine->AddOnScreenDebugMessage(-1, 0.001f, FColor::Yellow, "IsConfirmTargetingAllowed: " + FString::FromInt(TargetActor->IsConfirmTargetingAllowed()));
		GEngine->AddOnScreenDebugMessage(-1, 0.001f, FColor::Yellow, "GetTargetLocation: " + TargetActor->GetTargetLocation().ToString());
		GEngine->AddOnScreenDebugMessage(-1, 0.001f, FColor::Yellow, "TargetDataReadyDelegate.IsBound: " + FString::FromInt(TargetActor->TargetDataReadyDelegate.IsBound()));
		GEngine->AddOnScreenDebugMessage(-1, 0.001f, FColor::Yellow, "CanceledDelegate.IsBound: " + FString::FromInt(TargetActor->CanceledDelegate.IsBound()));

		TargetActor->ConfirmTargetingAndContinue();
		TargetActor->bDebug = TargetingParams.bDebug;
	}
	else
	{
		bIsFinished = true;

		EndTask();
	}
}