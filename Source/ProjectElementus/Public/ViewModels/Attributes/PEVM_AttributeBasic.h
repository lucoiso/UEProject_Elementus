// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "ViewModels/Attributes/PEVM_AttributeBase.h"
#include "PEVM_AttributeBasic.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Category = "Project Elementus | Classes")
class PROJECTELEMENTUS_API UPEVM_AttributeBasic : public UPEVM_AttributeBase
{
	GENERATED_BODY()
	
public:
	explicit UPEVM_AttributeBasic(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintPure, FieldNotify, Category = "Project Elementus | Functions")
	float GetHealthPercent() const;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, Category = "Project Elementus | Properties")
	float Health;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, Category = "Project Elementus | Properties", Category = "Project Elementus | Properties")
	float MaxHealth;

	UFUNCTION(BlueprintPure, FieldNotify, Category = "Project Elementus | Functions")
	float GetManaPercent() const;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, Category = "Project Elementus | Properties")
	float Mana;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, Category = "Project Elementus | Properties")
	float MaxMana;

	UFUNCTION(BlueprintPure, FieldNotify, Category = "Project Elementus | Functions")
	float GetStaminaPercent() const;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, Category = "Project Elementus | Properties")
	float Stamina;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, Category = "Project Elementus | Properties")
	float MaxStamina;

	virtual void OnAttributeChange(const FOnAttributeChangeData& AttributeChangeData);

private:
	void SetHealth(const float InValue);
	float GetHealth() const;

	void SetMaxHealth(const float InValue);
	float GetMaxHealth() const;

	void SetMana(const float InValue);
	float GetMana() const;

	void SetMaxMana(const float InValue);
	float GetMaxMana() const;

	void SetStamina(const float InValue);
	float GetStamina() const;

	void SetMaxStamina(const float InValue);
	float GetMaxStamina() const;
};
