#include "GAS/System/GASAbilitySystemComponent.h"

UGASAbilitySystemComponent::UGASAbilitySystemComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicated(true);
	ReplicationMode = EGameplayEffectReplicationMode::Minimal;
}
