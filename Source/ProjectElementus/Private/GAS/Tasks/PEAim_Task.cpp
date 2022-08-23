// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "GAS/Tasks/PEAim_Task.h"

UPEAim_Task::UPEAim_Task(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

UPEAim_Task* UPEAim_Task::Aim(UGameplayAbility* OwningAbility, const FName TaskInstanceName)
{
	UPEAim_Task* MyObj = NewAbilityTask<UPEAim_Task>(OwningAbility, TaskInstanceName);
	
	return MyObj;
}
