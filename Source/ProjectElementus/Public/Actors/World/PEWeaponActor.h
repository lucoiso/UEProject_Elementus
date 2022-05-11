// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PEWeaponActor.generated.h"

/**
 *
 */
UCLASS(Abstract, Blueprintable, Category = "Project Elementus | Classes")
class PROJECTELEMENTUS_API APEWeaponActor final : public AActor
{
	GENERATED_BODY()

public:
	explicit APEWeaponActor(const FObjectInitializer& ObjectInitializer);

	FORCEINLINE virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("Weapon Actor", GetFName());
	}
};
