// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GASAttributeData.generated.h"

/**
 *
 */
USTRUCT(BlueprintType, Category = "Custom GAS | Data")
struct FGASLevelingData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	FGASLevelingData() :
		Level(0),
		BonusMaxHealth(0.f),
		BonusMaxMana(0.f),
		BonusMaxStamina(0.f),
		BonusAttackRate(0.f),
		BonusDefenseRate(0.f),
		ExperienceNeeded(0.f)
	{
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BonusMaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BonusMaxMana;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BonusMaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BonusAttackRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BonusDefenseRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ExperienceNeeded;
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
