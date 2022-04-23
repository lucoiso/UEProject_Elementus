// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GAS/System/PEAttributeData.h"
#include "PEBasicStatusAS.generated.h"

/**
 *
 */
UCLASS(NotBlueprintable, NotPlaceable, Category = "Custom GAS | Attributes")
class PROJECTELEMENTUS_API UPEBasicStatusAS : public UAttributeSet
{
	GENERATED_BODY()

public:
	UPEBasicStatusAS(const FObjectInitializer& ObjectInitializer);

private:
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/* A helper function to clamp attribute values */
	void AdjustAttributeForMaxChange(const FGameplayAttributeData& AffectedAttribute,
		const FGameplayAttributeData& MaxAttribute,
		float NewMaxValue,
		const FGameplayAttribute& AffectedAttributeProperty) const;

public:
	/* A non-replicated attribute to handle damage value inside GE Executions */
	UPROPERTY(BlueprintReadOnly, Category = "Custom GAS | Attributes")
		FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UPEBasicStatusAS, Damage)

		UPROPERTY(BlueprintReadOnly, Category = "Custom GAS | Attributes", ReplicatedUsing = OnRep_Health)
		FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UPEBasicStatusAS, Health)

		UPROPERTY(BlueprintReadOnly, Category = "Custom GAS | Attributes", ReplicatedUsing = OnRep_MaxHealth)
		FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UPEBasicStatusAS, MaxHealth)

		UPROPERTY(BlueprintReadOnly, Category = "Custom GAS | Attributes", ReplicatedUsing = OnRep_Stamina)
		FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UPEBasicStatusAS, Stamina)

		UPROPERTY(BlueprintReadOnly, Category = "Custom GAS | Attributes", ReplicatedUsing = OnRep_MaxStamina)
		FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UPEBasicStatusAS, MaxStamina)

		UPROPERTY(BlueprintReadOnly, Category = "Custom GAS | Attributes", ReplicatedUsing = OnRep_Mana)
		FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UPEBasicStatusAS, Mana)

		UPROPERTY(BlueprintReadOnly, Category = "Custom GAS | Attributes", ReplicatedUsing = OnRep_MaxMana)
		FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UPEBasicStatusAS, MaxMana)

protected:
	UFUNCTION()
		void OnRep_Health(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
		void OnRep_MaxHealth(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
		void OnRep_Mana(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
		void OnRep_MaxMana(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
		void OnRep_Stamina(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
		void OnRep_MaxStamina(const FGameplayAttributeData& OldValue) const;
};