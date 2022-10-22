// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "GAS/Attributes/PEAttributeBase.h"
#include "PELevelingAS.generated.h"

/**
 *
 */
UCLASS(NotBlueprintable, NotPlaceable, Category = "Project Elementus | Classes")
class PROJECTELEMENTUS_API UPELevelingAS final : public UPEAttributeBase
{
	GENERATED_BODY()

public:
	explicit UPELevelingAS(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

private:
	TSoftObjectPtr<UDataTable> LevelingBonusData;

	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPROPERTY(BlueprintReadOnly, Category = "Project Elementus | Properties", ReplicatedUsing = OnRep_CurrentLevel)
	FGameplayAttributeData CurrentLevel;
	ATTRIBUTE_ACCESSORS(UPELevelingAS, CurrentLevel)

	UPROPERTY(BlueprintReadOnly, Category = "Project Elementus | Properties", ReplicatedUsing = OnRep_CurrentExperience)
	FGameplayAttributeData CurrentExperience;
	ATTRIBUTE_ACCESSORS(UPELevelingAS, CurrentExperience)

	UPROPERTY(BlueprintReadOnly, Category = "Project Elementus | Properties", ReplicatedUsing = OnRep_RequiredExperience)
	FGameplayAttributeData RequiredExperience;
	ATTRIBUTE_ACCESSORS(UPELevelingAS, RequiredExperience)

protected:
	UFUNCTION()
	void OnRep_CurrentLevel(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_CurrentExperience(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_RequiredExperience(const FGameplayAttributeData& OldValue) const;
};
