#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponActor.generated.h"

/**
 *
 */
UCLASS(Abstract, Blueprintable, Category = "Custom Classes | Actors")
class PROJECTELEMENTUS_API AWeaponActor : public AActor
{
	GENERATED_BODY()

public:
	AWeaponActor(const FObjectInitializer& ObjectInitializer);

	FORCEINLINE virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("Weapon Actor", GetFName());
	}
};
