// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEModularFeatures_ExtraActions

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AbilityInputBinding.generated.h"

/**
 * 
 */
UINTERFACE(MinimalAPI, BlueprintType, Category = "GFA Extra Actions | Modular Interfaces")
class UAbilityInputBinding : public UInterface
{
	GENERATED_BODY()
};

/* Your pawn or controller need this inferface to accept ability input bindings */
class MODULARFEATURES_EXTRAACTIONS_API IAbilityInputBinding
{
	GENERATED_IINTERFACE_BODY()

	/* This function is needed for setup ability input binding inside your controller or pawn */
	UFUNCTION(Category = "GFA Extra Actions | Modular Interfaces")
	virtual void SetupAbilityInput(class UInputAction* Action, const int32 InputID)
	{
	};

	/* This function is needed for removing ability input binding inside your controller or pawn */
	UFUNCTION(Category = "GFA Extra Actions | Modular Interfaces")
	virtual void RemoveAbilityInputBinding(const UInputAction* Action) const
	{
	};
};
