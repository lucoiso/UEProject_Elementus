// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ElementusInventoryComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Category = "Project Elementus | Classes")
class ELEMENTUSINVENTORY_API UElementusInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UElementusInventoryComponent();
};
