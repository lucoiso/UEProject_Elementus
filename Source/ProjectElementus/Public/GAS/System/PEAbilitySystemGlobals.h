// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "PEAbilitySystemGlobals.generated.h"

/**
 * 
 */
UCLASS(Category = "Custom GAS | Classes")
class PROJECTELEMENTUS_API UPEAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()

public:
	UPEAbilitySystemGlobals(const FObjectInitializer& ObjectInitializer);	
	
	TSoftClassPtr<class UGameplayEffect> GlobalDeathEffect;
	TSoftClassPtr<class UGameplayEffect> GlobalStunEffect;
	
	TSoftObjectPtr<class UDataTable> MainStatusAttributeData;
	TSoftObjectPtr<class UDataTable> CustomStatusAttributeData;
	TSoftObjectPtr<class UDataTable> LevelingAttributeData;
	TSoftObjectPtr<class UDataTable> LevelingBonusData;

	UFUNCTION(BlueprintPure, Category = "Custom GAS | Functions")
		UGameplayEffect* GetGlobalDeathEffect();
	
	UFUNCTION(BlueprintPure, Category = "Custom GAS | Functions")
		UGameplayEffect* GetGlobalStunEffect();

	UFUNCTION(BlueprintPure, Category = "Custom GAS | Functions")
		UDataTable* GetMainStatusAttributeMetaData();

	UFUNCTION(BlueprintPure, Category = "Custom GAS | Functions")
		UDataTable* GetCustomStatusAttributeMetaData();

	UFUNCTION(BlueprintPure, Category = "Custom GAS | Functions")
		UDataTable* GetLevelingAttributeMetaData();

	UFUNCTION(BlueprintPure, Category = "Custom GAS | Functions")
		UDataTable* GetLevelingBonusData();

	static UPEAbilitySystemGlobals& Get()
	{
		return *Cast<UPEAbilitySystemGlobals>(IGameplayAbilitiesModule::Get().GetAbilitySystemGlobals());
	}
};
