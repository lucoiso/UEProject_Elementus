// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "ViewModels/Attributes/PEVM_AttributeBasic.h"
#include "GAS/Attributes/PEBasicStatusAS.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PEVM_AttributeBasic)

UPEVM_AttributeBasic::UPEVM_AttributeBasic(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), Health(-1.f), MaxHealth(-1.f), Mana(-1.f), MaxMana(-1.f), Stamina(-1.f), MaxStamina(-1.f)
{
}

void UPEVM_AttributeBasic::NotifyAttributeChange(const FGameplayAttribute& Attribute, const float& NewValue)
{
	Super::NotifyAttributeChange(Attribute, NewValue);

	CHECK_ATTRIBUTE_AND_SET_VALUE(UPEBasicStatusAS, Health);
	CHECK_ATTRIBUTE_AND_SET_VALUE(UPEBasicStatusAS, MaxHealth);

	CHECK_ATTRIBUTE_AND_SET_VALUE(UPEBasicStatusAS, Mana);
	CHECK_ATTRIBUTE_AND_SET_VALUE(UPEBasicStatusAS, MaxMana);

	CHECK_ATTRIBUTE_AND_SET_VALUE(UPEBasicStatusAS, Stamina);
	CHECK_ATTRIBUTE_AND_SET_VALUE(UPEBasicStatusAS, MaxStamina);
}

float UPEVM_AttributeBasic::GetHealthPercent() const
{
	GET_MVVM_PERCENTAGE_VALUE(Health, MaxHealth);
}

void UPEVM_AttributeBasic::SetHealth(const float Value)
{
	UPDATE_MVVM_PROPERTY_VALUE_WITH_PERCENT(Health, Value);
}

float UPEVM_AttributeBasic::GetHealth() const
{
	return Health;
}

void UPEVM_AttributeBasic::SetMaxHealth(const float Value)
{
	UPDATE_MVVM_PROPERTY_MAX_VALUE_WITH_PERCENT(Health, Value);
}

float UPEVM_AttributeBasic::GetMaxHealth() const
{
	return MaxHealth;
}

float UPEVM_AttributeBasic::GetManaPercent() const
{
	GET_MVVM_PERCENTAGE_VALUE(Mana, MaxMana);
}

void UPEVM_AttributeBasic::SetMana(const float Value)
{
	UPDATE_MVVM_PROPERTY_VALUE_WITH_PERCENT(Mana, Value);
}

float UPEVM_AttributeBasic::GetMana() const
{
	return Mana;
}

void UPEVM_AttributeBasic::SetMaxMana(const float Value)
{
	UPDATE_MVVM_PROPERTY_MAX_VALUE_WITH_PERCENT(Mana, Value);
}

float UPEVM_AttributeBasic::GetMaxMana() const
{
	return MaxMana;
}

float UPEVM_AttributeBasic::GetStaminaPercent() const
{
	GET_MVVM_PERCENTAGE_VALUE(Stamina, MaxStamina);
}

void UPEVM_AttributeBasic::SetStamina(const float Value)
{
	UPDATE_MVVM_PROPERTY_VALUE_WITH_PERCENT(Stamina, Value);
}

float UPEVM_AttributeBasic::GetStamina() const
{
	return Stamina;
}

void UPEVM_AttributeBasic::SetMaxStamina(const float Value)
{
	UPDATE_MVVM_PROPERTY_MAX_VALUE_WITH_PERCENT(Stamina, Value);
}

float UPEVM_AttributeBasic::GetMaxStamina() const
{
	return MaxStamina;
}
