// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "Abilities/GameplayAbilityTargetDataFilter.h"
#include "Abilities/GameplayAbilityWorldReticle.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "CoreMinimal.h"
#include "GASTrace.generated.h"

/**
 *
 */
USTRUCT(BlueprintType, Category = "Custom GAS | Data")
struct PROJECTELEMENTUS_API FTargetActorSpawnParams
{
	GENERATED_USTRUCT_BODY()

public:
	FTargetActorSpawnParams() :
		Range(0.f),
		Height(0.f),
		Radius(0.f),
		StartLocation(FGameplayAbilityTargetingLocationInfo()),
		ReticleClass(AGameplayAbilityWorldReticle::StaticClass()),
		ReticleParams(FWorldReticleParameters()),
		TargetFilter(FGameplayTargetDataFilter()),
		bDestroyOnConfirmation(false),
		bTraceAffectsAimPitch(false),
		bDebug(false)
	{
	}

	float Range;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Height;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Radius;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		struct FGameplayAbilityTargetingLocationInfo StartLocation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TSubclassOf<class AGameplayAbilityWorldReticle> ReticleClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		struct FWorldReticleParameters ReticleParams;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		struct FGameplayTargetDataFilter TargetFilter;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool bDestroyOnConfirmation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool bTraceAffectsAimPitch;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool bDebug;
};