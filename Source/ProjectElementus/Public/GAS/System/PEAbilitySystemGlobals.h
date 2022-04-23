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
class PROJECTELEMENTUS_API UPEAbilitySystemGlobals final : public UAbilitySystemGlobals
{
	GENERATED_BODY()

public:
	explicit UPEAbilitySystemGlobals(const FObjectInitializer& ObjectInitializer);

	TSoftClassPtr<UGameplayEffect> GlobalDeathEffect;
	TSoftClassPtr<UGameplayEffect> GlobalStunEffect;

	TSoftObjectPtr<UDataTable> MainStatusAttributeData;
	TSoftObjectPtr<UDataTable> CustomStatusAttributeData;
	TSoftObjectPtr<UDataTable> LevelingAttributeData;
	TSoftObjectPtr<UDataTable> LevelingBonusData;

	UFUNCTION(BlueprintPure, Category = "Custom GAS | Functions")
	UGameplayEffect* GetGlobalDeathEffect() const;

	UFUNCTION(BlueprintPure, Category = "Custom GAS | Functions")
	UGameplayEffect* GetGlobalStunEffect() const;

	UFUNCTION(BlueprintPure, Category = "Custom GAS | Functions")
	UDataTable* GetMainStatusAttributeMetaData() const;

	UFUNCTION(BlueprintPure, Category = "Custom GAS | Functions")
	UDataTable* GetCustomStatusAttributeMetaData() const;

	UFUNCTION(BlueprintPure, Category = "Custom GAS | Functions")
	UDataTable* GetLevelingAttributeMetaData() const;

	UFUNCTION(BlueprintPure, Category = "Custom GAS | Functions")
	UDataTable* GetLevelingBonusData() const;

	static UPEAbilitySystemGlobals& Get()
	{
		return *Cast<UPEAbilitySystemGlobals>(IGameplayAbilitiesModule::Get().GetAbilitySystemGlobals());
	}
};
