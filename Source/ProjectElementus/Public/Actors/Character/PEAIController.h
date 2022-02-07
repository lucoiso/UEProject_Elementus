#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "PEAIController.generated.h"

/**
 *
 */
UCLASS(NotBlueprintable, NotPlaceable, Category = "Custom Classes | Player")
class PROJECTELEMENTUS_API APEAIController : public AAIController
{
	GENERATED_BODY()

public:
	APEAIController(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
