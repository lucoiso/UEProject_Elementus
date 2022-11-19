// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "PEVM_Settings.generated.h"

/**
 * 
 */
UCLASS(NotPlaceable, Category = "Project Elementus | Classes")
class PROJECTELEMENTUS_API UPEVM_Settings : public UMVVMViewModelBase
{
	GENERATED_BODY()
	
public:
	explicit UPEVM_Settings(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
