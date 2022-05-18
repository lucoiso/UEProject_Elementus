// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "PEHookAbility_Task.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FHookManagement, const bool, ValidTarget);

/**
 *
 */
UCLASS(MinimalAPI, NotBlueprintable, NotPlaceable, Category = "Project Elementus | Classes")
class UPEHookAbility_Task final : public UAbilityTask
{
	GENERATED_BODY()

public:
	explicit UPEHookAbility_Task(const FObjectInitializer& ObjectInitializer);

	FHookManagement OnHooking;

	/* Create a reference to manage this ability task */
	static UPEHookAbility_Task* HookAbilityMovement(UGameplayAbility* OwningAbility,
	                                                FName TaskInstanceName,
	                                                const FHitResult HitResult,
	                                                const float HookIntensity);

	virtual void Activate() override;

	FVector GetLastHookLocation() const;
	FHitResult GetHitResult() const;

private:
	virtual void TickTask(float DeltaTime) override;
	virtual void OnDestroy(bool AbilityIsEnding) override;

	TWeakObjectPtr<class APECharacter> HookOwner;
	TWeakObjectPtr<APECharacter> HitTarget;

	float Intensity;
	bool bIsFinished;
	FHitResult HitDataHandle;
	FVector CurrentHookLocation;
};
