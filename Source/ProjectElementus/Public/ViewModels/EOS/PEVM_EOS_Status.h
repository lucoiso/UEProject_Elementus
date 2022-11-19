// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "PEVM_EOS_Status.generated.h"

/**
 * 
 */
UCLASS(NotPlaceable, Category = "Project Elementus | Classes")
class PROJECTELEMENTUS_API UPEVM_EOS_Status : public UMVVMViewModelBase
{
	GENERATED_BODY()
	
public:
	explicit UPEVM_EOS_Status(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
