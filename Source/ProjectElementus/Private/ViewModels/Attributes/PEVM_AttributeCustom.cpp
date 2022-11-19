// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "ViewModels/Attributes/PEVM_AttributeCustom.h"
#include "GAS/Attributes/PECustomStatusAS.h"

UPEVM_AttributeCustom::UPEVM_AttributeCustom(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), AttackRate(-1.f), DefenseRate(-1.f), SpeedRate(-1.f), JumpRate(-1.f), Gold(-1.f)
{	
}

void UPEVM_AttributeCustom::OnAttributeChange(const FOnAttributeChangeData& AttributeChangeData)
{
}

void UPEVM_AttributeCustom::SetAttackRate(const float InValue)
{
	if (InValue == AttackRate)
	{
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(AttackRate, InValue);
}

float UPEVM_AttributeCustom::GetAttackRate() const
{
	return AttackRate;
}

void UPEVM_AttributeCustom::SetDefenseRate(const float InValue)
{
	if (InValue == DefenseRate)
	{
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(DefenseRate, InValue);
}

float UPEVM_AttributeCustom::GetDefenseRate() const
{
	return DefenseRate;
}

void UPEVM_AttributeCustom::SetSpeedRate(const float InValue)
{
	if (InValue == SpeedRate)
	{
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(SpeedRate, InValue);
}

float UPEVM_AttributeCustom::GetSpeedRate() const
{
	return SpeedRate;
}

void UPEVM_AttributeCustom::SetJumpRate(const float InValue)
{
	if (InValue == JumpRate)
	{
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(JumpRate, InValue);
}

float UPEVM_AttributeCustom::GetJumpRate() const
{
	return JumpRate;
}

void UPEVM_AttributeCustom::SetGold(const float InValue)
{
	if (InValue == Gold)
	{
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(Gold, InValue);
}

float UPEVM_AttributeCustom::GetGold() const
{
	return Gold;
}
