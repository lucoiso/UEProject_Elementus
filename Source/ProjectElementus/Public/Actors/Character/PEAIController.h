// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include <CoreMinimal.h>
#include <AIController.h>
#include "PEAIController.generated.h"

/**
 *
 */
UCLASS(NotBlueprintable, NotPlaceable, Category = "Project Elementus | Classes")
class PROJECTELEMENTUS_API APEAIController final : public AAIController
{
	GENERATED_BODY()

public:
	explicit APEAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
