// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "ThrowableActor.generated.h"

/**
 *
 */
UCLASS(MinimalAPI, Abstract, Blueprintable, Category = "Custom Classes | Actors")
class AThrowableActor : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	AThrowableActor(const FObjectInitializer& ObjectInitializer);

	FORCEINLINE virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("Throwable Actor", GetFName());
	}
};
