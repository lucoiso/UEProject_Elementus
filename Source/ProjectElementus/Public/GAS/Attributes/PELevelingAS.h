// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GAS/System/PEAttributeData.h"
#include "PELevelingAS.generated.h"

/**
 *
 */
UCLASS(NotBlueprintable, NotPlaceable, Category = "Custom GAS | Attributes")
class PROJECTELEMENTUS_API UPELevelingAS : public UAttributeSet
{
	GENERATED_BODY()

public:
	UPELevelingAS(const FObjectInitializer& ObjectInitializer);

private:
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPROPERTY(BlueprintReadOnly, Category = "Custom GAS | Attributes", ReplicatedUsing = OnRep_CurrentLevel)
		FGameplayAttributeData CurrentLevel;
	ATTRIBUTE_ACCESSORS(UPELevelingAS, CurrentLevel)

		UPROPERTY(BlueprintReadOnly, Category = "Custom GAS | Attributes", ReplicatedUsing = OnRep_CurrentExperience)
		FGameplayAttributeData CurrentExperience;
	ATTRIBUTE_ACCESSORS(UPELevelingAS, CurrentExperience)

		UPROPERTY(BlueprintReadOnly, Category = "Custom GAS | Attributes", ReplicatedUsing = OnRep_RequiredExperience)
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
