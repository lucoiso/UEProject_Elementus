// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "ViewModels/Attributes/PEVM_AttributeLeveling.h"
#include "GAS/Attributes/PELevelingAS.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PEVM_AttributeLeveling)

UPEVM_AttributeLeveling::UPEVM_AttributeLeveling(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), CurrentLevel(-1.f), CurrentExperience(-1.f), RequiredExperience(-1.f)
{	
}

void UPEVM_AttributeLeveling::NotifyAttributeChange(const FGameplayAttribute& Attribute, const float& NewValue)
{
	Super::NotifyAttributeChange(Attribute, NewValue);

	CHECK_ATTRIBUTE_AND_SET_VALUE(UPELevelingAS, CurrentLevel);
	CHECK_ATTRIBUTE_AND_SET_VALUE(UPELevelingAS, CurrentExperience);
	CHECK_ATTRIBUTE_AND_SET_VALUE(UPELevelingAS, RequiredExperience);
}

float UPEVM_AttributeLeveling::GetExperiencePercent() const
{
	GET_MVVM_PERCENTAGE_VALUE(CurrentExperience, RequiredExperience);
}

void UPEVM_AttributeLeveling::SetCurrentLevel(const float Value)
{
	UPDATE_MVVM_PROPERTY_VALUE(CurrentLevel, Value);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetExperiencePercent);
}

float UPEVM_AttributeLeveling::GetCurrentLevel() const
{
	return CurrentLevel;
}

void UPEVM_AttributeLeveling::SetCurrentExperience(const float Value)
{
	UPDATE_MVVM_PROPERTY_VALUE(CurrentExperience, Value);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetExperiencePercent);
}

float UPEVM_AttributeLeveling::GetCurrentExperience() const
{
	return CurrentExperience;
}

void UPEVM_AttributeLeveling::SetRequiredExperience(const float Value)
{
	UPDATE_MVVM_PROPERTY_VALUE(RequiredExperience, Value);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetExperiencePercent);
}

float UPEVM_AttributeLeveling::GetRequiredExperience() const
{
	return RequiredExperience;
}
