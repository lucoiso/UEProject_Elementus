// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PEInteractable.generated.h"

class APECharacter;
/**
 *
 */
UINTERFACE(MinimalAPI, Blueprintable, Category = "Custom GAS | Interfaces")
class UPEInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class PROJECTELEMENTUS_API IPEInteractable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Custom Functions | Behaviors")
	void DoInteractionBehavior(APECharacter* CharacterInteracting, const FHitResult& HitResult);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Custom Functions | Management")
	void SetIsCurrentlyFocusedByActor(const bool bIsFocused, AActor* ActorFocusing, const FHitResult& HitResult);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Custom Functions | Management")
	bool IsInteractEnabled();
};
