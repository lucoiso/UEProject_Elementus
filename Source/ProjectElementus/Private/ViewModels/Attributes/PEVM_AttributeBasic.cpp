// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "ViewModels/Attributes/PEVM_AttributeBasic.h"
#include "GAS/Attributes/PEBasicStatusAS.h"

UPEVM_AttributeBasic::UPEVM_AttributeBasic(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), Health(-1.f), MaxHealth(-1.f), Mana(-1.f), MaxMana(-1.f), Stamina(-1.f), MaxStamina(-1.f)
{
}

float UPEVM_AttributeBasic::GetHealthPercent() const
{
	if (Health <= 0.f || MaxHealth <= 0.f)
	{
		return 0.f;
	}
	
	return Health / MaxHealth;
}

#define CHECK_ATTRIBUTE_AND_SET_VALUE(AttributeName) \
if (AttributeChangeData.Attribute == UPEBasicStatusAS::Get##AttributeName##Attribute()) \
{ \
	SetHealth(AttributeChangeData.NewValue); \
	return; \
}

void UPEVM_AttributeBasic::OnAttributeChange(const FOnAttributeChangeData& AttributeChangeData)
{
	CHECK_ATTRIBUTE_AND_SET_VALUE(Health);
	CHECK_ATTRIBUTE_AND_SET_VALUE(MaxHealth);

	CHECK_ATTRIBUTE_AND_SET_VALUE(Mana);
	CHECK_ATTRIBUTE_AND_SET_VALUE(MaxMana);

	CHECK_ATTRIBUTE_AND_SET_VALUE(Stamina);
	CHECK_ATTRIBUTE_AND_SET_VALUE(MaxStamina);
}
#undef CHECK_ATTRIBUTE_AND_SET_VALUE

void UPEVM_AttributeBasic::SetHealth(const float InValue)
{
	if (InValue == Health)
	{
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(Health, InValue);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetHealthPercent);
}

float UPEVM_AttributeBasic::GetHealth() const
{
	return Health;
}

void UPEVM_AttributeBasic::SetMaxHealth(const float InValue)
{
	if (InValue == MaxHealth)
	{
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(MaxHealth, InValue);
}

float UPEVM_AttributeBasic::GetMaxHealth() const
{
	return MaxHealth;
}

float UPEVM_AttributeBasic::GetManaPercent() const
{
	if (Mana <= 0.f || MaxMana <= 0.f)
	{
		return 0.f;
	}

	return Mana / MaxMana;
}

void UPEVM_AttributeBasic::SetMana(const float InValue)
{
	if (InValue == Mana)
	{
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(Mana, InValue);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetManaPercent);
}

float UPEVM_AttributeBasic::GetMana() const
{
	return Mana;
}

void UPEVM_AttributeBasic::SetMaxMana(const float InValue)
{
	if (InValue == MaxMana)
	{
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(MaxMana, InValue);
}

float UPEVM_AttributeBasic::GetMaxMana() const
{
	return MaxMana;
}

float UPEVM_AttributeBasic::GetStaminaPercent() const
{
	if (Stamina <= 0.f || MaxStamina <= 0.f)
	{
		return 0.f;
	}

	return Stamina / MaxStamina;
}

void UPEVM_AttributeBasic::SetStamina(const float InValue)
{
	if (InValue == Stamina)
	{
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(Stamina, InValue);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetStaminaPercent);
}

float UPEVM_AttributeBasic::GetStamina() const
{
	return Stamina;
}

void UPEVM_AttributeBasic::SetMaxStamina(const float InValue)
{
	if (InValue == MaxStamina)
	{
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(MaxStamina, InValue);
}

float UPEVM_AttributeBasic::GetMaxStamina() const
{
	return MaxStamina;
}
