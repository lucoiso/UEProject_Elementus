// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "ViewModels/Attributes/PEVM_AttributeBase.h"

#if UE_WITH_IRIS
#include <Iris/ReplicationSystem/ReplicationFragmentUtil.h>
#endif // UE_WITH_IRIS

#include UE_INLINE_GENERATED_CPP_BY_NAME(PEVM_AttributeBase)

UPEVM_AttributeBase::UPEVM_AttributeBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UPEVM_AttributeBase::NotifyAttributeChange(const FGameplayAttribute& Attribute, const float& NewValue)
{	
}

#if UE_WITH_IRIS
void UPEVM_AttributeBase::RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags)
{
	Super::RegisterReplicationFragments(Context, RegistrationFlags);
	UE::Net::FReplicationFragmentUtil::CreateAndRegisterFragmentsForObject(this, Context, RegistrationFlags);
}
#endif // UE_WITH_IRIS
