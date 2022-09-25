// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Components/TimelineComponent.h"
#include "PEAim_Task.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAimDelegate);
/**
 * 
 */
UCLASS(NotBlueprintable, NotPlaceable, Category = "Project Elementus | Classes")
class PROJECTELEMENTUS_API UPEAim_Task final : public UAbilityTask
{
	GENERATED_BODY()

public:
	explicit UPEAim_Task(const FObjectInitializer& ObjectInitializer);

	/* Create a reference to manage this ability task */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UPEAim_Task* Aim(UGameplayAbility* OwningAbility, const FName TaskInstanceName, const FVector CameraRelativeTargetPosition, const float CameraLerpTime = 1.f, const bool bAdjustTimeToCurrentLocation = true);

	UPROPERTY(BlueprintAssignable)
	FAimDelegate OnCompleted;

	UPROPERTY(BlueprintAssignable)
	FAimDelegate OnReversionCompleted;
	
	UPROPERTY(BlueprintAssignable)
	FAimDelegate OnFailed;

	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	void RevertCameraPosition();
	
	virtual void Activate() override;
	virtual void OnDestroy(const bool AbilityIsEnding) override;

private:
	FVector CameraInitialPosition;
	FVector CameraTargetPosition;
	float CameraLerpTime;
	bool bAdjustTimeToCurrentLocation;

	float CurrentValue = 0.f;

	TWeakObjectPtr<class UCameraComponent> TargetCamera;
	TWeakObjectPtr<class UTimelineComponent> TaskTimeline;

protected:
	UFUNCTION()
	virtual void TimelineProgress(const float InValue);
	
	UFUNCTION()
	virtual void TimelineFinished();
};
