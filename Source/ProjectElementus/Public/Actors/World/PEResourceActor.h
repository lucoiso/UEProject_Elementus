// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PEResourceActor.generated.h"

/**
 *
 */
UCLASS(Abstract, Blueprintable, Category = "Project Elementus | Classes")
class PROJECTELEMENTUS_API APEResourceActor : public AActor
{
	GENERATED_BODY()

public:
	explicit APEResourceActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
