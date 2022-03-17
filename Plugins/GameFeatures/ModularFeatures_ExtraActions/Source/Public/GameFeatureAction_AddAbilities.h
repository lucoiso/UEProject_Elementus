// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEModularFeatures_ExtraActions

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpec.h"
#include "GameFeatureAction_WorldActionBase.h"
#include "GameFeatureAction_AddAbilities.generated.h"

class UGameplayAbility;
class UInputAction;
struct FComponentRequestHandle;
/**
 * 
 */
USTRUCT(BlueprintType, Category = "GFA Extra Actions | Modular Structs")
struct FAbilityMapping
{
	GENERATED_BODY()

	/* Ability class to be added */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftClassPtr<UGameplayAbility> AbilityClass;

	/* Enhanced Input Action to bind ability activation */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UInputAction> InputAction;

	/* Ability Level */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 AbilityLevel = 1;

	/* InputID Value Name associated by Enumeration Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "InputID Value Name"))
	FName InputIDValueName;
};


/**
 * 
 */
UCLASS(MinimalAPI, meta = (DisplayName = "Add Abilities"))
class UGameFeatureAction_AddAbilities final : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()

public:
	/* Target actor to which abilities will be given */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Settings", meta = (OnlyPlaceable))
	TSoftClassPtr<AActor> TargetActorClass;

	/* Determines whether the binding will be performed within the controller class or within the pawn */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Settings")
	EControllerOwner InputBindingOwner = EControllerOwner::Controller;

	/* Enumeration class that will be used by the Ability System Component to manage abilities inputs */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Settings", meta = (DisplayName = "InputID Enumeration Class"))
	TSoftObjectPtr<UEnum> InputIDEnumerationClass;

	/* Abilities to be added */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Settings", meta=(DisplayName="Ability Mapping", ShowOnlyInnerProperties))
	TArray<FAbilityMapping> Abilities;

protected:
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
	virtual void AddToWorld(const FWorldContext& WorldContext) override;

private:
	void HandleActorExtension(AActor* Owner, FName EventName);
	void ResetExtension();

	UFUNCTION(Server, Reliable)
	void AddActorAbilities(AActor* TargetActor, const FAbilityMapping& Ability);
	void AddActorAbilities_Implementation(AActor* TargetActor, const FAbilityMapping& Ability);

	UFUNCTION(Server, Reliable)
	void RemoveActorAbilities(AActor* TargetActor);
	void RemoveActorAbilities_Implementation(AActor* TargetActor);

	struct FActiveAbilityData
	{
		TArray<FGameplayAbilitySpecHandle> SpecHandle;
		TArray<UInputAction*> InputReference;
	};

	TMap<TWeakObjectPtr<AActor>, FActiveAbilityData> ActiveExtensions;
	TArray<TSharedPtr<FComponentRequestHandle>> ActiveRequests;
};
