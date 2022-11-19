// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "ViewModels/Attributes/PEVM_AttributeLeveling.h"
#include "GAS/Attributes/PELevelingAS.h"

UPEVM_AttributeLeveling::UPEVM_AttributeLeveling(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), CurrentLevel(-1.f), CurrentExperience(-1.f), RequiredExperience(-1.f)
{	
}

float UPEVM_AttributeLeveling::GetExperiencePercent() const
{
	if (CurrentExperience <= 0.f || RequiredExperience <= 0.f)
	{
		return 0.f;
	}

	return CurrentExperience / RequiredExperience;
}

void UPEVM_AttributeLeveling::OnAttributeChange(const FOnAttributeChangeData& AttributeChangeData)
{
}

void UPEVM_AttributeLeveling::SetCurrentLevel(const float InValue)
{
	if (InValue == CurrentLevel)
	{
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(CurrentLevel, InValue);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetExperiencePercent);
}

float UPEVM_AttributeLeveling::GetCurrentLevel() const
{
	return CurrentLevel;
}

void UPEVM_AttributeLeveling::SetCurrentExperience(const float InValue)
{
	if (InValue == CurrentExperience)
	{
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(CurrentExperience, InValue);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetExperiencePercent);
}

float UPEVM_AttributeLeveling::GetCurrentExperience() const
{
	return CurrentExperience;
}

void UPEVM_AttributeLeveling::SetRequiredExperience(const float InValue)
{
	if (InValue == RequiredExperience)
	{
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(RequiredExperience, InValue);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetExperiencePercent);
}

float UPEVM_AttributeLeveling::GetRequiredExperience() const
{
	return RequiredExperience;
}
