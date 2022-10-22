// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "GameFeatureAction_AddInputs.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PEPlayerLibrary.generated.h"

USTRUCT(BlueprintType, Category = "Project Elementus | Structs")
struct FPlayerInputBindingHandle
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Project Elementus | Properties")
	TObjectPtr<APlayerController> PlayerController = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Project Elementus | Properties")
	int32 InputBindingHandle = 0;
};

/**
 * 
 */
UCLASS(Category = "Project Elementus | Classes")
class PROJECTELEMENTUS_API UPEPlayerLibrary final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/* Dynamicaly bind a new enhanced input to the player */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	static FPlayerInputBindingHandle BindDynamicInput(APlayerController* Controller, UInputAction* Action, UObject* Object, const FName UFunctionName, const ETriggerEvent TriggerEvent);

	/* Dynamicaly remove a enhanced input binding from player */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	static void RemoveDynamicInput(const FPlayerInputBindingHandle BindingHandle);

	/* Dynamicaly bind a new enhanced input mapping context to the player */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	static void AddDynamicMapping(APlayerController* Controller, UInputMappingContext* InputMapping, const int32 Priority);

	/* Dynamicaly remove a enhanced input mapping context from the player */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	static void RemoveDynamicMapping(APlayerController* Controller, UInputMappingContext* InputMapping);

	/* Check if the player contains the specified bindings and return the handle */
	UFUNCTION(BlueprintPure, Category = "Project Elementus | Functions")
	static bool CheckIfPlayerContainsDynamicInput(APlayerController* Controller, TArray<FPlayerInputBindingHandle> BindingArray, FPlayerInputBindingHandle& BindingHandle);

	UFUNCTION(BlueprintPure, Category = "Project Elementus | Functions")
	static FVector GetSocketLocationInChildMeshes(const USkeletalMeshComponent* ParentMesh, const FName SocketName);
};
