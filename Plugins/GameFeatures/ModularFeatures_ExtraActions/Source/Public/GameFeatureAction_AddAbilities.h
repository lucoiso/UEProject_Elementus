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

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftClassPtr<UGameplayAbility> AbilityClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UInputAction> InputAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 AbilityLevel = 1;

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
	UPROPERTY(EditAnywhere, Category="Settings")
	TSoftClassPtr<AActor> TargetActorClass;

	UPROPERTY(EditAnywhere, Category="Settings")
	EControllerOwner InputBindingOwner = EControllerOwner::Controller;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "InputID Enumeration Class"), Category="Settings")
	TSoftObjectPtr<UEnum> InputIDEnumerationClass;

	UPROPERTY(EditAnywhere, Category="Settings", meta=(TitleProperty="Ability Mapping", ShowOnlyInnerProperties))
	TArray<FAbilityMapping> Abilities;

protected:
	virtual void OnGameFeatureActivating() override;
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
