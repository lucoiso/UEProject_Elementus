#pragma once

#include "CoreMinimal.h"
#include "InputTriggers.h"
#include "EnhancedInputComponent.h"
#include "GameFeatureAction_WorldActionBase.h"
#include "GameFeatureAction_AddInputs.generated.h"

class UInputMappingContext;
struct FComponentRequestHandle;

/**
 * 
 */

USTRUCT(BlueprintType, Category = "GFA Extra Actions | Modular Structs")
struct FFunctionStackedData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName FunctionName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<ETriggerEvent> Triggers;
};

USTRUCT(BlueprintType, Category = "GFA Extra Actions | Modular Structs")
struct FInputMappingStack
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UInputAction> ActionInput;

	/* UFunction and Triggers to bind activation by Enhanced Input */
	UPROPERTY(EditAnywhere, meta = (DisplayName = "UFunctions to Bind"))
	TArray<FFunctionStackedData> FunctionBindingData;
};

/**
 * 
 */
UCLASS(MinimalAPI, meta = (DisplayName = "Add Enhanced Input Mapping"))
class UGameFeatureAction_AddInputs final : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Settings")
	TSoftClassPtr<APawn> TargetPawnClass;

	UPROPERTY(EditAnywhere, Category="Settings")
	EControllerOwner InputBindingOwner = EControllerOwner::Controller;

	UPROPERTY(EditAnywhere, Category="Settings")
	TSoftObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditAnywhere, Category="Settings")
	int32 MappingPriority = 1;

	UPROPERTY(EditAnywhere, Category="Settings", meta=(DisplayName="Actions Bindings", ShowOnlyInnerProperties))
	TArray<FInputMappingStack> ActionsBindings;

protected:
	virtual void OnGameFeatureActivating() override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
	virtual void AddToWorld(const FWorldContext& WorldContext) override;

private:
	void HandleActorExtension(AActor* Owner, FName EventName);
	void ResetExtension();

	UFUNCTION(Client, Reliable)
	void AddActorInputs(AActor* TargetActor);
	void AddActorInputs_Implementation(AActor* TargetActor);

	UFUNCTION(Client, Reliable)
	void RemoveActorInputs(AActor* TargetActor);
	void RemoveActorInputs_Implementation(AActor* TargetActor);

	struct FInputBindingData
	{
		TArray<FInputBindingHandle> ActionBinding;
		TWeakObjectPtr<UInputMappingContext> Mapping;
	};

	TMap<TWeakObjectPtr<AActor>, FInputBindingData> ActiveExtensions;
	TArray<TSharedPtr<FComponentRequestHandle>> ActiveRequests;
};
