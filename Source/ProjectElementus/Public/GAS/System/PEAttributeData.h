// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "PEAttributeData.generated.h"

/**
 *
 */
USTRUCT(BlueprintType, Category = "Project Elementus | Structs | Data")
struct FPELevelingData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	FPELevelingData() :
		BonusMaxHealth(0.f),
		BonusMaxMana(0.f),
		BonusMaxStamina(0.f),
		BonusAttackRate(0.f),
		BonusDefenseRate(0.f),
		RequiredExp(0.f)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float BonusMaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float BonusMaxMana;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float BonusMaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float BonusAttackRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float BonusDefenseRate;

	/* Required experience to pass to next level */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Required Xp to Level Up"), Category = "Settings")
	float RequiredExp;
};

/**
 *
 */
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 *
 */
