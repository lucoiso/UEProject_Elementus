// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "GameplayEffectTypes.h"
#include "PEVM_AttributeBase.generated.h"

#define CHECK_ATTRIBUTE_AND_SET_VALUE(AttributeClass, AttributeName) \
if (AttributeChangeData.Attribute == ##AttributeClass##::Get##AttributeName##Attribute()) \
{ \
	Set##AttributeName##(AttributeChangeData.NewValue); \
	return; \
}

/**
 * 
 */
UCLASS(Abstract, BlueprintType, Category = "Project Elementus | Classes")
class PROJECTELEMENTUS_API UPEVM_AttributeBase : public UMVVMViewModelBase
{
	GENERATED_BODY()
	
public:
	explicit UPEVM_AttributeBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void OnAttributeChange(const FOnAttributeChangeData& AttributeChangeData);
};
