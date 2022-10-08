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

	UPROPERTY(EditAnywhere, Category = "Project Elementus | Properties", meta = (TitleProperty = "{InputID} -> {Ability Class}"))
	TMap<FName, TSubclassOf<UGameplayAbility>> EquipmentAbilities;

	UPROPERTY(EditAnywhere, Category = "Project Elementus | Properties")
	TSoftObjectPtr<USkeletalMesh> EquipmentMesh;

	UPROPERTY(EditAnywhere, Category = "Project Elementus | Properties")
	FName SocketToAttach = NAME_None;
};
