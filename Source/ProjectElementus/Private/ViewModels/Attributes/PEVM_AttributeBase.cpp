// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "ViewModels/Attributes/PEVM_AttributeBase.h"

UPEVM_AttributeBase::UPEVM_AttributeBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UPEVM_AttributeBase::OnAttributeChange(const FOnAttributeChangeData& AttributeChangeData)
{
	GEngine->AddOnScreenDebugMessage((int32)GetUniqueID(), 5.f, FColor::Yellow, FString::Printf(TEXT("Attribute: %s; Value: %f"), *AttributeChangeData.Attribute.AttributeName, AttributeChangeData.NewValue));
}
