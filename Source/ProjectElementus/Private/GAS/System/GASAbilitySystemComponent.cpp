// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "GAS/System/GASAbilitySystemComponent.h"

UGASAbilitySystemComponent::UGASAbilitySystemComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicated(true);
	ReplicationMode = EGameplayEffectReplicationMode::Minimal;
}

void UGASAbilitySystemComponent::ApplyEffectGroupedDataToSelf(FGameplayEffectGroupedData GroupedData)
{	
	if (IsOwnerActorAuthoritative())
	{
		const FGameplayEffectSpecHandle& SpecHandle = MakeOutgoingSpec(GroupedData.EffectClass, 1.f, MakeEffectContext());

		for (const TPair<FGameplayTag, float>& StackedData : GroupedData.SetByCallerStackedData)
		{
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(StackedData.Key,
				StackedData.Value);
		}

		if (SpecHandle.IsValid())
		{
			ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
		}
	}
}

void UGASAbilitySystemComponent::ApplyEffectGroupedDataToTarget(FGameplayEffectGroupedData GroupedData, UAbilitySystemComponent* TargetABSC)
{
	if (IsOwnerActorAuthoritative())
	{
		const FGameplayEffectSpecHandle& SpecHandle = MakeOutgoingSpec(GroupedData.EffectClass, 1.f, MakeEffectContext());

		for (const TPair<FGameplayTag, float>& StackedData : GroupedData.SetByCallerStackedData)
		{
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(StackedData.Key,
				StackedData.Value);
		}

		if (SpecHandle.IsValid())
		{
			ApplyGameplayEffectSpecToTarget(*SpecHandle.Data, TargetABSC);
		}
	}
}
