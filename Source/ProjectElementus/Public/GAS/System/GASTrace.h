// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "Abilities/GameplayAbilityTargetDataFilter.h"
#include "CoreMinimal.h"
#include "GASTrace.generated.h"

/**
 *
 */
UENUM(BlueprintType, Category = "Custom GAS | Enumerations")
enum class EGASTraceType : uint8
{
	Single,
	Area
};

USTRUCT(BlueprintType, Category = "Custom GAS | Data")
struct PROJECTELEMENTUS_API FGASTraceDataHandle
{
	GENERATED_USTRUCT_BODY()

	FGASTraceDataHandle();

	FHitResult Hit;
	float MaxRange;
	TWeakObjectPtr<USceneComponent> Component;
	bool bDoTick;
	EGASTraceType Type;
	FGameplayTargetDataFilterHandle FilterHandle;
	FCollisionQueryParams QueryParams;
	FName CollisionProfile;
};
