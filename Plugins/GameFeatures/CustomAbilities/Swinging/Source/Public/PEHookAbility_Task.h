// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include <CoreMinimal.h>
#include <Abilities/Tasks/AbilityTask.h>
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
	explicit UPEHookAbility_Task(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	FHookManagement OnHooking;

	/* Create a reference to manage this ability task */
	static UPEHookAbility_Task* HookAbilityMovement(UGameplayAbility* OwningAbility, const FName TaskInstanceName, const FHitResult HitResult, const float HookIntensity, const float HookMaxForce = -1.f);

	virtual void Activate() override;

	FVector GetLastHookLocation() const;
	FHitResult GetHitResult() const;

protected:
	virtual void TickTask(float DeltaTime) override;
	virtual void OnDestroy(bool AbilityIsEnding) override;

private:
	TWeakObjectPtr<class ACharacter> HookOwner;
	TWeakObjectPtr<ACharacter> HitTarget;

	float Intensity;
	float MaxForce;
	bool bIsFinished;
	FHitResult HitDataHandle;
	FVector CurrentHookLocation;

	bool bIsTargetMovable;
	bool bIsGeometryCollection;

	void UpdateHookMovement();
};
