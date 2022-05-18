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
UCLASS(Category = "Project Elementus | Classes")
class PROJECTELEMENTUS_API UPEAbilitySystemGlobals final : public UAbilitySystemGlobals
{
	GENERATED_BODY()

public:
	explicit UPEAbilitySystemGlobals(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintPure, Category = "Project Elementus | Functions")
	UGameplayEffect* GetGlobalDeathEffect() const;

	UFUNCTION(BlueprintPure, Category = "Project Elementus | Functions")
	UGameplayEffect* GetGlobalStunEffect() const;

	UFUNCTION(BlueprintPure, Category = "Project Elementus | Functions")
	UDataTable* GetMainStatusAttributeMetaData() const;

	UFUNCTION(BlueprintPure, Category = "Project Elementus | Functions")
	UDataTable* GetCustomStatusAttributeMetaData() const;

	UFUNCTION(BlueprintPure, Category = "Project Elementus | Functions")
	UDataTable* GetLevelingAttributeMetaData() const;

	UFUNCTION(BlueprintPure, Category = "Project Elementus | Functions")
	UDataTable* GetLevelingBonusData() const;

	UFUNCTION(BlueprintPure, Category = "Project Elementus | Functions")
	UEnum* GetMainInputIDEnum() const;

	static UPEAbilitySystemGlobals& Get()
	{
		return *Cast<UPEAbilitySystemGlobals>(IGameplayAbilitiesModule::Get().GetAbilitySystemGlobals());
	}

private:
	TSoftClassPtr<UGameplayEffect> GlobalDeathEffect;
	TSoftClassPtr<UGameplayEffect> GlobalStunEffect;

	TSoftObjectPtr<UDataTable> MainStatusAttributeData;
	TSoftObjectPtr<UDataTable> CustomStatusAttributeData;
	TSoftObjectPtr<UDataTable> LevelingAttributeData;
	TSoftObjectPtr<UDataTable> LevelingBonusData;
	TSoftObjectPtr<UEnum> MainInputIDEnum;
};
