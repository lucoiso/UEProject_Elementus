// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ResourceActor.generated.h"

/**
 *
 */
UCLASS(Abstract, Blueprintable, Category = "Custom Classes | Actors")
class PROJECTELEMENTUS_API AResourceActor : public AActor
{
	GENERATED_BODY()

public:
	AResourceActor(const FObjectInitializer& ObjectInitializer);

	FORCEINLINE virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("Resource Actor", GetFName());
	}
};
