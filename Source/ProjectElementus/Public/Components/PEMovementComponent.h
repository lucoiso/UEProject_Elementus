// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PEMovementComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup = (Custom), Category = "Project Elementus | Classes", EditInlineNew, meta = (BlueprintSpawnableComponent))
class PROJECTELEMENTUS_API UPEMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	explicit UPEMovementComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
