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
		BonusHealth(0.f),
		BonusMana(0.f),
		BonusStamina(0.f),
		BonusAttackRate(0.f),
		BonusDefenseRate(0.f),
		ExperienceNeeded(0.f)
	{
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BonusHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BonusMana;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BonusStamina;

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
