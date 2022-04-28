// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Actors/Character/PECharacter.h"
#include "PEInteractAbility_Task.generated.h"

/**
 *
 */
UCLASS(NotBlueprintable, NotPlaceable, Category = "Custom GAS | Tasks")
class DEFAULTABILITIES_API UPEInteractAbility_Task final : public UAbilityTask
{
	GENERATED_BODY()

public:
	explicit UPEInteractAbility_Task(const FObjectInitializer& ObjectInitializer);

	/* Create a reference to manage this ability task */
	static UPEInteractAbility_Task* InteractionTask(UGameplayAbility* OwningAbility, FName TaskInstanceName,
	                                                const float InteractionRange);

	virtual void Activate() override;

	bool GetIsInteractAllowed() const;

	AActor* GetInteractable() const;

private:
	virtual void TickTask(float DeltaTime) override;
	virtual void OnDestroy(bool AbilityIsEnding) override;

	UFUNCTION()
	void OnCannotInteractChanged();

	bool bIsFinished;
	float Range;

	TWeakObjectPtr<APECharacter> InteractionOwner;
	TWeakObjectPtr<AActor> LastInteractable_Ref;
};
