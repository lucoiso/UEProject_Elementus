// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "TelekinesisAbility_Task.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FTelekinesisManagement, const bool, ValidTarget);
/**
 *
 */
UCLASS(MinimalAPI, NotBlueprintable, NotPlaceable, Category = "Custom GAS | Tasks")
class UTelekinesisAbility_Task : public UAbilityTask
{
	GENERATED_BODY()

public:
	UTelekinesisAbility_Task(const FObjectInitializer& ObjectInitializer);

	FTelekinesisManagement OnGrabbing;

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
	TWeakObjectPtr<class APECharacterBase> TelekinesisOwner;
	TWeakObjectPtr<AActor> TelekinesisTarget;
};
