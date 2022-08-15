// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "GAS/System/PEAbilityData.h"
#include "Components/ActorComponent.h"
#include "PEEquipment.generated.h"

struct FGameplayEffectGroupedData;
/**
 *
 */
UCLASS(Abstract, Blueprintable, Category = "Project Elementus | Classes")
class UPEEquipment : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Project Elementus | Properties")
	TArray<FGameplayEffectGroupedData> EquipmentEffects;
};
