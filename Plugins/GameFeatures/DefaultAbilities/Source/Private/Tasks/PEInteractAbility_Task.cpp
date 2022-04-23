// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Tasks/PEInteractAbility_Task.h"

UPEInteractAbility_Task::UPEInteractAbility_Task(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = false;
	bIsFinished = false;
}

UPEInteractAbility_Task* UPEInteractAbility_Task::InteractionTask(UGameplayAbility* OwningAbility,
                                                                  const FName TaskInstanceName)
{
	UPEInteractAbility_Task* MyObj = NewAbilityTask<UPEInteractAbility_Task>(OwningAbility, TaskInstanceName);
	return MyObj;
}

void UPEInteractAbility_Task::Activate()
{
	Super::Activate();

	bIsFinished = true;
	EndTask();
}

void UPEInteractAbility_Task::TickTask(const float DeltaTime)
{
	if (bIsFinished)
	{
		EndTask();
		return;
	}

	Super::TickTask(DeltaTime);
}

void UPEInteractAbility_Task::OnDestroy(const bool AbilityIsEnding)
{
	UE_LOG(LogGameplayTasks, Warning, TEXT("Task %s ended"), *GetName());

	bIsFinished = true;

	Super::OnDestroy(AbilityIsEnding);
}
