// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "ViewModels/Attributes/PEVM_AttributeBase.h"
#include "PEVM_AttributeLeveling.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Category = "Project Elementus | Classes")
class PROJECTELEMENTUS_API UPEVM_AttributeLeveling : public UPEVM_AttributeBase
{
	GENERATED_BODY()
	
public:
	explicit UPEVM_AttributeLeveling(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, Category = "Project Elementus | Properties")
	float CurrentLevel;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, Category = "Project Elementus | Properties")
	float CurrentExperience;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, Category = "Project Elementus | Properties")
	float RequiredExperience;

	UFUNCTION(BlueprintPure, FieldNotify, Category = "Project Elementus | Functions")
	float GetExperiencePercent() const;
	
	virtual void OnAttributeChange(const FOnAttributeChangeData& AttributeChangeData);

private:
	void SetCurrentLevel(const float InValue);
	float GetCurrentLevel() const;

	void SetCurrentExperience(const float InValue);
	float GetCurrentExperience() const;

	void SetRequiredExperience(const float InValue);
	float GetRequiredExperience() const;
};
