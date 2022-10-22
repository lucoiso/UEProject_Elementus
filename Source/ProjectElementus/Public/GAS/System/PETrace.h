// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "Abilities/GameplayAbilityTargetDataFilter.h"
#include "Abilities/GameplayAbilityWorldReticle.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "CoreMinimal.h"
#include "PETrace.generated.h"

/**
 *
 */
USTRUCT(BlueprintType, Category = "Project Elementus | Structs")
struct PROJECTELEMENTUS_API FTargetActorSpawnParams
{
	GENERATED_USTRUCT_BODY()

	FTargetActorSpawnParams() : Range(0.f), Height(0.f), Radius(0.f), StartLocation(FGameplayAbilityTargetingLocationInfo()), ReticleClass(AGameplayAbilityWorldReticle::StaticClass()), ReticleParams(FWorldReticleParameters()), TargetFilter(FGameplayTargetDataFilter()), bDestroyOnConfirmation(false), bTraceAffectsAimPitch(false), bDebug(false)
	{
	}

	float Range;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Project Elementus | Properties")
	float Height;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Project Elementus | Properties")
	float Radius;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Project Elementus | Properties")
	FGameplayAbilityTargetingLocationInfo StartLocation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Project Elementus | Properties")
	TSubclassOf<AGameplayAbilityWorldReticle> ReticleClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Project Elementus | Properties")
	FWorldReticleParameters ReticleParams;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Project Elementus | Properties")
	FGameplayTargetDataFilter TargetFilter;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Project Elementus | Properties")
	bool bDestroyOnConfirmation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Project Elementus | Properties")
	bool bTraceAffectsAimPitch;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Project Elementus | Properties")
	bool bDebug;
};
