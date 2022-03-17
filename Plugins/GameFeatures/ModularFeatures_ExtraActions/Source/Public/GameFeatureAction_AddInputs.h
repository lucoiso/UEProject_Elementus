// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEModularFeatures_ExtraActions

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

	/* UFunction name */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName FunctionName;

	/* Input Trigger event type */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<ETriggerEvent> Triggers;
};

USTRUCT(BlueprintType, Category = "GFA Extra Actions | Modular Structs")
struct FInputMappingStack
{
	GENERATED_BODY()

	/* Enhanced Input Action to bind with these settings */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UInputAction> ActionInput;

	/* UFunction and Triggers to bind activation by Enhanced Input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "UFunction Bindings"))
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
	/* Target pawn to which Input Mapping will be given */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Settings", meta = (AllowedClasses="Pawn"))
	TSoftClassPtr<APawn> TargetPawnClass;
	
	/* Determines whether the binding will be performed within the controller class or within the pawn */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Settings")
	EControllerOwner InputBindingOwner = EControllerOwner::Controller;

	/* Enhanced Input Mapping Context to be added */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Settings")
	TSoftObjectPtr<UInputMappingContext> InputMappingContext;

	/* Input Mapping priority */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Settings")
	int32 MappingPriority = 1;

	/* Enhanced Input Actions binding stacked data */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Settings", meta=(DisplayName="Actions Bindings", ShowOnlyInnerProperties))
	TArray<FInputMappingStack> ActionsBindings;

protected:
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
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
