// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GAS/System/PETrace.h"
#include "PEAbilityFunctions.generated.h"

/**
 * 
 */
UCLASS(Category = "Custom GAS | Classes")
class PROJECTELEMENTUS_API UPEAbilityFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Custom GAS | Helpers | Management")
		static const FGameplayAbilityTargetDataHandle MakeTargetDataHandleFromSingleHitResult(const FHitResult HitResult);

	UFUNCTION(BlueprintPure, Category = "Custom GAS | Helpers | Management")
		static const FGameplayAbilityTargetDataHandle MakeTargetDataHandleFromHitResultArray(const TArray<FHitResult> HitResults);

	UFUNCTION(BlueprintPure, Category = "Custom GAS | Helpers | Management")
		static const FGameplayAbilityTargetDataHandle MakeTargetDataHandleFromActorArray(const TArray<AActor*> TargetActors);
};
