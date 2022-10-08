// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GAS/System/PEAbilityData.h"
#include "PEEquipment.generated.h"

class APECharacter;

/**
 *
 */
UCLASS(Abstract, Blueprintable, Category = "Project Elementus | Classes")
class UPEEquipment : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Project Elementus | Properties")
	TArray<FGameplayEffectGroupedData> EquipmentEffects;

	UPROPERTY(EditAnywhere, Category = "Project Elementus | Properties")
	FGameplayTagContainer EquipmentSlotTags;

	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	virtual bool ProcessEquipmentApplication(APECharacter* EquipmentOwner);

	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	virtual bool ProcessEquipmentRemoval(APECharacter* EquipmentOwner);
};
