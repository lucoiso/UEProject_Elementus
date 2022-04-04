// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "GAS/System/GASTrace.h"
#include "Targeting_Task.generated.h"

/**
 *
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitTargetingDelegate, const struct FGameplayAbilityTargetDataHandle&, Data);

/**
 *
 */
UCLASS(NotBlueprintable, NotPlaceable, Category = "Custom GAS | Tasks")
class PROJECTELEMENTUS_API UTargeting_Task : public UAbilityTask
{
	GENERATED_BODY()

public:
	UTargeting_Task(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintAssignable)
		FWaitTargetingDelegate ValidData;

	UPROPERTY(BlueprintAssignable)
		FWaitTargetingDelegate Cancelled;

	/** Spawns target actor and waits for it to return valid data or to be canceled. */
	UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true",
		HideSpawnParms = "Instigator", AutoCreateRefTerm = "Parameters"), Category = "Custom GAS | Tasks")
		static UTargeting_Task* StartTargetingAndWaitData(UGameplayAbility* OwningAbility, const FName TaskInstanceName,
			const TEnumAsByte<EGameplayTargetingConfirmation::Type> ConfirmationType,
			const TSubclassOf<AGameplayAbilityTargetActor_Trace> TargetActorClass,
			const FTargetActorSpawnParams Parameters);

protected:
	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;
	virtual void OnDestroy(bool AbilityEnded) override;

private:
	TSubclassOf<AGameplayAbilityTargetActor_Trace> TargetActorClass;
	TWeakObjectPtr<class AGameplayAbilityTargetActor> TargetActor;
	TEnumAsByte<EGameplayTargetingConfirmation::Type> ConfirmationType;
	FTargetActorSpawnParams TargetingParams;

	bool bIsFinished;

	UFUNCTION()
		virtual void OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& Data);

	UFUNCTION()
		virtual void OnTargetDataCancelledCallback(const FGameplayAbilityTargetDataHandle& Data);
};
