// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "GAS/System/PETrace.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PEAbilityFunctions.generated.h"

/**
 *
 */
UCLASS(Category = "Project Elementus | Classes")
class PROJECTELEMENTUS_API UPEAbilityFunctions final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/* Create a FGameplayAbilityTargetDataHandle with the specified HitResult data */
	UFUNCTION(BlueprintPure, Category = "Project Elementus | Functions")
	static FGameplayAbilityTargetDataHandle MakeTargetDataHandleFromSingleHitResult(const FHitResult HitResult);

	/* Create a FGameplayAbilityTargetDataHandle with the specified HitResults data */
	UFUNCTION(BlueprintPure, Category = "Project Elementus | Functions")
	static FGameplayAbilityTargetDataHandle MakeTargetDataHandleFromHitResultArray(const TArray<FHitResult> HitResults);

	/* Create a FGameplayAbilityTargetDataHandle with the specified Target Actors */
	UFUNCTION(BlueprintPure, Category = "Project Elementus | Functions")
	static FGameplayAbilityTargetDataHandle MakeTargetDataHandleFromActorArray(const TArray<AActor*> TargetActors);

	/* Check if the ABSC contains a ability with the given class */
	UFUNCTION(BlueprintPure, Category = "Project Elementus | Functions")
	static bool HasAbilityWithClass(UAbilitySystemComponent* TargetABSC, const TSubclassOf<UGameplayAbility> AbilityClass);

	/* Give a new ability without binding a input */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	static void GiveAbilityWithoutBinding(UAbilitySystemComponent* TargetABSC, const TSubclassOf<UGameplayAbility> Ability, const bool bTryRemoveExistingAbilityWithClass);

	/* Give a new Ability to the Player -
	 * bAutoAdjustInput will ignore InputId and select Skill_1, Skill_2 or Skill_3 based on current owned abilities */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	static void GiveAbility(UAbilitySystemComponent* TargetABSC, const TSubclassOf<UGameplayAbility> Ability, const FName InputId, const UEnum* EnumerationClass, const bool bTryRemoveExistingAbilityWithInput, const bool bTryRemoveExistingAbilityWithClass);

	/* Will remove the ability associated to the InputAction */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	static void RemoveAbility(UAbilitySystemComponent* TargetABSC, const TSubclassOf<UGameplayAbility> Ability);
};
