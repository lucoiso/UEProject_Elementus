// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "PEInteractAbility_Task.generated.h"

/**
 *
 */
UCLASS(NotBlueprintable, NotPlaceable, Category = "Project Elementus | Classes")
class DEFAULTABILITIES_API UPEInteractAbility_Task final : public UAbilityTask
{
	GENERATED_BODY()

public:
	explicit UPEInteractAbility_Task(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/* Create a reference to manage this ability task */
	static UPEInteractAbility_Task* InteractionTask(UGameplayAbility* OwningAbility, const FName& TaskInstanceName, const float InteractionRange, const bool bUseCustomDepth = false);

	virtual void Activate() override;

	bool GetIsInteractAllowed() const;

	AActor* GetInteractable() const;

	FHitResult GetInteractableHitResult() const;

private:
	virtual void TickTask(float DeltaTime) override;
	virtual void OnDestroy(bool AbilityIsEnding) override;

	UFUNCTION()
	void OnCannotInteractChanged();

	bool bIsFinished;
	float Range;
	bool bUseCustomDepth;

	TWeakObjectPtr<class APECharacter> InteractionOwner;
	TWeakObjectPtr<AActor> LastInteractableActor_Ref;
	TWeakObjectPtr<UPrimitiveComponent> LastInteractablePrimitive_Ref;

	FHitResult HitResult;
};
