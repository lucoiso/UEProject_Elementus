#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Management/Data/GASAttributeData.h"
#include "GASAttributeSet.generated.h"

/**
 *
 */
UCLASS(NotBlueprintable, NotPlaceable, Category = "Custom GAS | Attributes")
class PROJECTELEMENTUS_API UGASAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UGASAttributeSet(const FObjectInitializer& ObjectInitializer);

private:
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void AdjustAttributeForMaxChange(const FGameplayAttributeData& AffectedAttribute,
	                                 const FGameplayAttributeData& MaxAttribute,
	                                 float NewMaxValue,
	                                 const FGameplayAttribute& AffectedAttributeProperty) const;

public:
	UPROPERTY(BlueprintReadOnly, Category = "Custom GAS | Attributes")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UGASAttributeSet, Damage)

	UPROPERTY(BlueprintReadOnly, Category = "Custom GAS | Attributes", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UGASAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, Category = "Custom GAS | Attributes", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UGASAttributeSet, MaxHealth)

	UPROPERTY(BlueprintReadOnly, Category = "Custom GAS | Attributes", ReplicatedUsing = OnRep_Stamina)
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UGASAttributeSet, Stamina)

	UPROPERTY(BlueprintReadOnly, Category = "Custom GAS | Attributes", ReplicatedUsing = OnRep_MaxStamina)
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UGASAttributeSet, MaxStamina)

	UPROPERTY(BlueprintReadOnly, Category = "Custom GAS | Attributes", ReplicatedUsing = OnRep_Mana)
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UGASAttributeSet, Mana)

	UPROPERTY(BlueprintReadOnly, Category = "Custom GAS | Attributes", ReplicatedUsing = OnRep_MaxMana)
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UGASAttributeSet, MaxMana)

	UPROPERTY(BlueprintReadOnly, Category = "Custom GAS | Attributes", ReplicatedUsing = OnRep_AttackRate)
	FGameplayAttributeData AttackRate;
	ATTRIBUTE_ACCESSORS(UGASAttributeSet, AttackRate)

	UPROPERTY(BlueprintReadOnly, Category = "Custom GAS | Attributes", ReplicatedUsing = OnRep_DefenseRate)
	FGameplayAttributeData DefenseRate;
	ATTRIBUTE_ACCESSORS(UGASAttributeSet, DefenseRate)

	UPROPERTY(BlueprintReadOnly, Category = "Custom GAS | Attributes", ReplicatedUsing = OnRep_SpeedRate)
	FGameplayAttributeData SpeedRate;
	ATTRIBUTE_ACCESSORS(UGASAttributeSet, SpeedRate)

	UPROPERTY(BlueprintReadOnly, Category = "Custom GAS | Attributes", ReplicatedUsing = OnRep_JumpRate)
	FGameplayAttributeData JumpRate;
	ATTRIBUTE_ACCESSORS(UGASAttributeSet, JumpRate)

	UPROPERTY(BlueprintReadOnly, Category = "Custom GAS | Attributes", ReplicatedUsing = OnRep_HealthRegenerationRate)
	FGameplayAttributeData HealthRegenerationRate;
	ATTRIBUTE_ACCESSORS(UGASAttributeSet, HealthRegenerationRate)

	UPROPERTY(BlueprintReadOnly, Category = "Custom GAS | Attributes", ReplicatedUsing = OnRep_ManaRegenerationRate)
	FGameplayAttributeData ManaRegenerationRate;
	ATTRIBUTE_ACCESSORS(UGASAttributeSet, ManaRegenerationRate)

	UPROPERTY(BlueprintReadOnly, Category = "Custom GAS | Attributes", ReplicatedUsing = OnRep_StaminaRegenerationRate)
	FGameplayAttributeData StaminaRegenerationRate;
	ATTRIBUTE_ACCESSORS(UGASAttributeSet, StaminaRegenerationRate)

	UPROPERTY(BlueprintReadOnly, Category = "Custom GAS | Attributes", ReplicatedUsing = OnRep_Level)
	FGameplayAttributeData Level;
	ATTRIBUTE_ACCESSORS(UGASAttributeSet, Level)

	UPROPERTY(BlueprintReadOnly, Category = "Custom GAS | Attributes", ReplicatedUsing = OnRep_Experience)
	FGameplayAttributeData Experience;
	ATTRIBUTE_ACCESSORS(UGASAttributeSet, Experience)

	UPROPERTY(BlueprintReadOnly, Category = "Custom GAS | Attributes", ReplicatedUsing = OnRep_Gold)
	FGameplayAttributeData Gold;
	ATTRIBUTE_ACCESSORS(UGASAttributeSet, Gold)

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

	UFUNCTION()
	void OnRep_AttackRate(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_DefenseRate(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_SpeedRate(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_JumpRate(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_HealthRegenerationRate(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_ManaRegenerationRate(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_StaminaRegenerationRate(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_Level(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_Experience(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_Gold(const FGameplayAttributeData& OldValue) const;
};
