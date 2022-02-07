#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "TelekinesisAbility_Task.generated.h"

/**
 *
 */
UCLASS(MinimalAPI, NotBlueprintable, NotPlaceable, Category = "Custom GAS | Tasks")
class UTelekinesisAbility_Task : public UAbilityTask
{
	GENERATED_BODY()

public:
	UTelekinesisAbility_Task(const FObjectInitializer& ObjectInitializer);

	/* Create a reference to manage this ability task */
	static UTelekinesisAbility_Task* TelekinesisAbilityMovement(UGameplayAbility* OwningAbility,
	                                                            FName TaskInstanceName,
	                                                            const TWeakObjectPtr<AActor> Target);

	virtual void Activate() override;

	void ThrowObject();

private:
	virtual void TickTask(float DeltaTime) override;

	virtual void OnDestroy(bool AbilityIsEnding) override;

protected:
	bool bIsFinished;

	TWeakObjectPtr<class UPhysicsHandleComponent> PhysicsHandle;
	TWeakObjectPtr<AActor> TelekinesisTarget;
};
