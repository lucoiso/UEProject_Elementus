// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "ElementusInventoryComponent.h"
#include "PEInventoryComponent.generated.h"

/**
 *
 */
UCLASS(Blueprintable, ClassGroup = (Custom), Category = "Project Elementus | Classes", meta = (BlueprintSpawnableComponent))
class PROJECTELEMENTUS_API UPEInventoryComponent : public UElementusInventoryComponent
{
	GENERATED_BODY()

public:
	explicit UPEInventoryComponent(const FObjectInitializer& ObjectInitializer);
	
	virtual bool CanGiveItem(const FElementusItemInfo InItemInfo) const override;
};
