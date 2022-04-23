// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "PEInteractableActor.generated.h"

/**
 *
 */
UCLASS(Abstract, Blueprintable, Category = "Custom Classes | Actors")
class PROJECTELEMENTUS_API APEInteractableActor : public AActor
{
	GENERATED_BODY()

public:
	APEInteractableActor(const FObjectInitializer& ObjectInitializer);

	FORCEINLINE virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("Interactable Actor", GetFName());
	}

	UFUNCTION(BlueprintCallable, Category = "Custom Functions | Behaviors")
		void PerformInteraction(class UAbilitySystemComponent* TargetABSC);

	UFUNCTION(BlueprintNativeEvent, Category = "Custom Functions | Behaviors")
		void DoInteractionBehavior();

	void DoInteractionBehavior_Implementation()
	{
		// Override on child classes
	};

protected:
	/* Tags that target require to interact with this */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom Properties | Defaults")
		FGameplayTagContainer RequirementsTags;
};