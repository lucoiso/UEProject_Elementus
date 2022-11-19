// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "ViewModels/Attributes/PEVM_AttributeBasic.h"

UPEVM_AttributeBasic::UPEVM_AttributeBasic(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), CurrentHealth(-1.f), MaxHealth(-1.f), CurrentMana(-1.f), MaxMana(-1.f), CurrentStamina(-1.f), MaxStamina(-1.f)
{
}

float UPEVM_AttributeBasic::GetHealthPercent() const
{
	if (CurrentHealth <= 0.f || MaxHealth <= 0.f)
	{
		return 0.f;
	}
	
	return CurrentHealth / MaxHealth;
}

void UPEVM_AttributeBasic::SetCurrentHealth(const float InValue)
{
	if (InValue == CurrentHealth)
	{
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(CurrentHealth, InValue);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetHealthPercent);
}

float UPEVM_AttributeBasic::GetCurrentHealth() const
{
	return CurrentHealth;
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
	if (CurrentMana <= 0.f || MaxMana <= 0.f)
	{
		return 0.f;
	}

	return CurrentMana / MaxMana;
}

void UPEVM_AttributeBasic::SetCurrentMana(const float InValue)
{
	if (InValue == CurrentMana)
	{
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(CurrentMana, InValue);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetManaPercent);
}

float UPEVM_AttributeBasic::GetCurrentMana() const
{
	return CurrentMana;
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
	if (CurrentStamina <= 0.f || MaxStamina <= 0.f)
	{
		return 0.f;
	}

	return CurrentStamina / MaxStamina;
}

void UPEVM_AttributeBasic::SetCurrentStamina(const float InValue)
{
	if (InValue == CurrentStamina)
	{
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(CurrentStamina, InValue);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetStaminaPercent);
}

float UPEVM_AttributeBasic::GetCurrentStamina() const
{
	return CurrentStamina;
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
