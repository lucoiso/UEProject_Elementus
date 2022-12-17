// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include <CoreMinimal.h>
#include <Abilities/Tasks/AbilityTask.h>
#include "PETelekinesisAbility_Task.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FTelekinesisManagement, const bool, ValidTarget);

/**
 *
 */
UCLASS(MinimalAPI, NotBlueprintable, NotPlaceable, Category = "Project Elementus | Classes")
class UPETelekinesisAbility_Task final : public UAbilityTask
{
	GENERATED_BODY()

public:
	explicit UPETelekinesisAbility_Task(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	FTelekinesisManagement OnGrabbing;

	/* Create a reference to manage this ability task */
	static UPETelekinesisAbility_Task* PETelekinesisAbilityMovement(UGameplayAbility* OwningAbility, const FName TaskInstanceName, const float ThrowIntensity, AActor* Target);

	virtual void Activate() override;

	void ThrowObject();

	AActor* GetTelekinesisTarget() const;

protected:
	virtual void TickTask(float DeltaTime) override;

	virtual void OnDestroy(bool AbilityIsEnding) override;

private:
	bool bIsFinished;
	float Intensity;

	TWeakObjectPtr<class UPhysicsHandleComponent> PhysicsHandle;
	TWeakObjectPtr<class APECharacter> TelekinesisOwner;
	TWeakObjectPtr<AActor> TelekinesisTarget;
};
