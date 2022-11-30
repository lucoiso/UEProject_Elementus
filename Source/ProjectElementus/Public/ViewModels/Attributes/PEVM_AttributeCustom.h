// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include <CoreMinimal.h>
#include "ViewModels/Attributes/PEVM_AttributeBase.h"
#include "PEVM_AttributeCustom.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Category = "Project Elementus | Classes")
class PROJECTELEMENTUS_API UPEVM_AttributeCustom : public UPEVM_AttributeBase
{
	GENERATED_BODY()
	
public:
	explicit UPEVM_AttributeCustom(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, Category = "Project Elementus | Properties")
	float AttackRate;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, Category = "Project Elementus | Properties")
	float DefenseRate;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, Category = "Project Elementus | Properties")
	float SpeedRate;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, Category = "Project Elementus | Properties")
	float JumpRate;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, Category = "Project Elementus | Properties")
	float Gold;

	virtual void OnAttributeChange(const FOnAttributeChangeData& AttributeChangeData);

private:
	void SetAttackRate(const float InValue);
	float GetAttackRate() const;

	void SetDefenseRate(const float InValue);
	float GetDefenseRate() const;

	void SetSpeedRate(const float InValue);
	float GetSpeedRate() const;

	void SetJumpRate(const float InValue);
	float GetJumpRate() const;

	void SetGold(const float InValue);
	float GetGold() const;
};
