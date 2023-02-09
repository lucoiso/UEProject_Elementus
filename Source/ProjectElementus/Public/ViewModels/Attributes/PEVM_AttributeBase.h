// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include <CoreMinimal.h>
#include <MVVMViewModelBase.h>
#include <GameplayEffectTypes.h>
#include "PEVM_AttributeBase.generated.h"

#define CHECK_ATTRIBUTE_AND_SET_VALUE(AttributeClass, AttributeName) \
if (Attribute == ##AttributeClass##::Get##AttributeName##Attribute()) \
{ \
	Set##AttributeName##(NewValue); \
	return; \
}

#define UPDATE_MVVM_PROPERTY_VALUE(MemberName, NewValue) \
if (NewValue == MemberName) \
{ \
	return; \
} \
UE_MVVM_SET_PROPERTY_VALUE(MemberName, NewValue);

#define UPDATE_MVVM_PROPERTY_VALUE_WITH_PERCENT(MemberName, NewValue) \
UPDATE_MVVM_PROPERTY_VALUE(MemberName, NewValue); \
UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(Get##MemberName##Percent);

#define UPDATE_MVVM_PROPERTY_MAX_VALUE(MemberName, NewValue) \
if (NewValue == Max##MemberName) \
{ \
	return; \
} \
UE_MVVM_SET_PROPERTY_VALUE(Max##MemberName, NewValue);

#define UPDATE_MVVM_PROPERTY_MAX_VALUE_WITH_PERCENT(MemberName, NewValue) \
UPDATE_MVVM_PROPERTY_MAX_VALUE(MemberName, NewValue); \
UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(Get##MemberName##Percent);

#define GET_MVVM_PERCENTAGE_VALUE(MemberName, MaxMemberName) \
if (MemberName <= 0.f || MaxMemberName <= 0.f) \
{ \
	return 0.f; \
} \
return MemberName / MaxMemberName;

/**
 * 
 */
UCLASS(Abstract, BlueprintType, Category = "Project Elementus | Classes")
class PROJECTELEMENTUS_API UPEVM_AttributeBase : public UMVVMViewModelBase
{
	GENERATED_BODY()

	friend class UPEAbilitySystemComponent;

public:
	explicit UPEVM_AttributeBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NotifyAttributeChange(const FGameplayAttribute& Attribute, const float& NewValue);

#if UE_WITH_IRIS
	virtual void RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags);
#endif // UE_WITH_IRIS
};
