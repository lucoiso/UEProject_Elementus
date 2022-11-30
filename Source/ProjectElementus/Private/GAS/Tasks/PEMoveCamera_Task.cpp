// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "GAS/Tasks/PEMoveCamera_Task.h"
#include "Actors/Character/PECharacter.h"
#include <Components/TimelineComponent.h>
#include <Camera/CameraComponent.h>

UPEMoveCamera_Task::UPEMoveCamera_Task(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bTickingTask = false;
}

UPEMoveCamera_Task* UPEMoveCamera_Task::MoveCamera(UGameplayAbility* OwningAbility, const FName TaskInstanceName, const FVector CameraRelativeTargetPosition, const float CameraLerpTime, const bool bAdjustTimeToCurrentLocation)
{
	UPEMoveCamera_Task* const MyObj = NewAbilityTask<UPEMoveCamera_Task>(OwningAbility, TaskInstanceName);	
	MyObj->CameraTargetPosition = CameraRelativeTargetPosition;
	MyObj->CameraLerpTime = CameraLerpTime;
	MyObj->bAdjustTimeToCurrentLocation = bAdjustTimeToCurrentLocation;
	
	return MyObj;
}

void UPEMoveCamera_Task::Activate()
{
	Super::Activate();

	check(Ability);

	TaskTimeline = NewObject<UTimelineComponent>(GetAvatarActor(), UTimelineComponent::StaticClass(), TEXT("MoveCameraTimeline"));
	if (!TaskTimeline.IsValid())
	{
		UE_LOG(LogGameplayTasks, Error, TEXT("%s - Task %s ended on activation due to invalid timeline"), *FString(__func__), *GetName());

		EndTask();
		return;
	}

	APECharacter* const OwningCharacter = Cast<APECharacter>(Ability->GetAvatarActorFromActorInfo());

	if (ensureAlwaysMsgf(IsValid(OwningCharacter), TEXT("%s - Task %s failed to activate because have a invalid owner"), *FString(__func__), *GetName()))
	{
		TargetCamera = OwningCharacter->GetFollowCamera();
		CameraInitialPosition = TargetCamera->GetRelativeLocation();

		if (!TargetCamera.IsValid())
		{
			UE_LOG(LogGameplayTasks, Error, TEXT("%s - Task %s ended on activation due to invalid camera target"), *FString(__func__), *GetName());
			
			EndTask();
		}

		if (bAdjustTimeToCurrentLocation)
		{
			const float Distance1 = FVector::Distance(CameraInitialPosition, CameraTargetPosition);
			const float Distance2 = FVector::Distance(APECharacter::GetCameraDefaultPosition(), CameraTargetPosition);
			CameraLerpTime *= (Distance1 / Distance2);
		}
		
		TaskTimeline->SetTimelineLength(CameraLerpTime);

		UCurveFloat* const TimelineCurve = NewObject<UCurveFloat>();
		TimelineCurve->FloatCurve.AddKey(0.f, 0.f);
		TimelineCurve->FloatCurve.AddKey(CameraLerpTime, 1.f);

		FOnTimelineFloat TimelineUpdate;
		TimelineUpdate.BindUFunction(this, TEXT("TimelineProgress"));
		TaskTimeline->AddInterpFloat(TimelineCurve, TimelineUpdate);
		
		FOnTimelineEvent TimelineFinished;
		TimelineFinished.BindUFunction(this, TEXT("TimelineFinished"));
		TaskTimeline->SetTimelineFinishedFunc(TimelineFinished);
		
		TaskTimeline->RegisterComponentWithWorld(GetWorld());
		TaskTimeline->Play();
	}
}

void UPEMoveCamera_Task::RevertCameraPosition()
{
	if (!TaskTimeline.IsValid())
	{
		UE_LOG(LogGameplayTasks, Error, TEXT("%s - Task %s failed while trying to revert camera position due to invalid timeline"), *FString(__func__), *GetName());
		
		OnFailed.Broadcast();
		EndTask();

		return;
	}
	
	TaskTimeline->Reverse();
}

void UPEMoveCamera_Task::OnDestroy(const bool AbilityIsEnding)
{
	UE_LOG(LogGameplayTasks, Display, TEXT("%s - Task %s ended"), *FString(__func__), *GetName());

	if (TaskTimeline.IsValid() && TaskTimeline->IsPlaying())
	{
		UE_LOG(LogGameplayTasks, Warning, TEXT("%s - Task %s ended while the timeline is playing!"), *FString(__func__), *GetName());
	}

	Super::OnDestroy(AbilityIsEnding);
}

void UPEMoveCamera_Task::TimelineProgress(const float InValue)
{
	CurrentValue = InValue;
	
	if (!TargetCamera.IsValid())
	{
		UE_LOG(LogGameplayTasks, Error, TEXT("%s - Task %s failed while trying to change camera position due to invalid target camera"), *FString(__func__), *GetName());
		
		OnFailed.Broadcast();
		
		TaskTimeline->Stop();
		EndTask();
		
		return;
	}

	TargetCamera->SetRelativeLocation(FMath::Lerp(CameraInitialPosition, CameraTargetPosition, InValue));
}

void UPEMoveCamera_Task::TimelineFinished()
{
	if (CurrentValue == 0.f)
	{
		OnReversionCompleted.Broadcast();
	}
	else if (CurrentValue == CameraLerpTime)
	{
		OnMoveCompleted.Broadcast();		
	}	
}
