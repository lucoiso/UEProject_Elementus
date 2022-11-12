// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "GAS/System/PEAbilitySystemComponent.h"
#include "GAS/System/PEAbilityData.h"
#include "GAS/System/PEEffectData.h"

UPEAbilitySystemComponent::UPEAbilitySystemComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SetIsReplicated(true);
	ReplicationMode = EGameplayEffectReplicationMode::Minimal;
}

void UPEAbilitySystemComponent::ApplyEffectGroupedDataToSelf(const FGameplayEffectGroupedData GroupedData)
{
	if (!IsOwnerActorAuthoritative())
	{
		return;
	}

	const FGameplayEffectSpecHandle& SpecHandle = MakeOutgoingSpec(GroupedData.EffectClass, 1.f, MakeEffectContext());

	for (const TPair<FGameplayTag, float>& StackedData : GroupedData.SetByCallerStackedData)
	{
		SpecHandle.Data.Get()->SetSetByCallerMagnitude(StackedData.Key, StackedData.Value);
	}

	if (SpecHandle.IsValid())
	{
		ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	}
}

void UPEAbilitySystemComponent::ApplyEffectGroupedDataToTarget(const FGameplayEffectGroupedData GroupedData, UAbilitySystemComponent* TargetABSC)
{
	if (!IsOwnerActorAuthoritative())
	{
		return;
	}

	const FGameplayEffectSpecHandle& SpecHandle = MakeOutgoingSpec(GroupedData.EffectClass, 1.f, MakeEffectContext());

	for (const TPair<FGameplayTag, float>& StackedData : GroupedData.SetByCallerStackedData)
	{
		SpecHandle.Data.Get()->SetSetByCallerMagnitude(StackedData.Key, StackedData.Value);
	}

	if (SpecHandle.IsValid())
	{
		ApplyGameplayEffectSpecToTarget(*SpecHandle.Data, TargetABSC);
	}
}

void UPEAbilitySystemComponent::RemoveEffectGroupedDataFromSelf(const FGameplayEffectGroupedData GroupedData, UAbilitySystemComponent* InstigatorABSC, const int32 StacksToRemove)
{
	if (!IsOwnerActorAuthoritative())
	{
		return;
	}
	
	if (!IsValid(GroupedData.EffectClass))
	{
		return;
	}
	
	FGameplayEffectQuery Query;
	Query.CustomMatchDelegate.BindLambda([&](const FActiveGameplayEffect& CurEffect)
	{
		bool bMatches = false;

		if (IsValid(CurEffect.Spec.Def) && GroupedData.EffectClass == CurEffect.Spec.Def->GetClass() && InstigatorABSC == CurEffect.Spec.GetEffectContext().GetInstigatorAbilitySystemComponent())
		{
			for (const TPair<FGameplayTag, float>& Iterator : GroupedData.SetByCallerStackedData)
			{
				bMatches = CurEffect.Spec.SetByCallerTagMagnitudes.FindRef(Iterator.Key) == Iterator.Value;

				if (!bMatches)
				{
					break;
				}
			}
		}

		return bMatches;
	});

	bIsNetDirty = true;
	RemoveActiveEffects(Query, StacksToRemove);
}

void UPEAbilitySystemComponent::RemoveEffectGroupedDataFromTarget(const FGameplayEffectGroupedData GroupedData, UAbilitySystemComponent* InstigatorABSC, UAbilitySystemComponent* TargetABSC, const int32 StacksToRemove)
{
	if (!IsOwnerActorAuthoritative())
	{
		return;
	}

	if (!IsValid(GroupedData.EffectClass))
	{
		return;
	}

	FGameplayEffectQuery Query;
	Query.CustomMatchDelegate.BindLambda([&](const FActiveGameplayEffect& CurEffect)
	{
		bool bMatches = false;

		if (IsValid(CurEffect.Spec.Def) && GroupedData.EffectClass == CurEffect.Spec.Def->GetClass() && InstigatorABSC == CurEffect.Spec.GetEffectContext().GetInstigatorAbilitySystemComponent())
		{
			for (const TPair<FGameplayTag, float>& Iterator : GroupedData.SetByCallerStackedData)
			{
				bMatches = CurEffect.Spec.SetByCallerTagMagnitudes.FindRef(Iterator.Key) == Iterator.Value;

				if (!bMatches)
				{
					break;
				}
			}
		}

		return bMatches;
	});

	bIsNetDirty = true;
	TargetABSC->RemoveActiveEffects(Query, StacksToRemove);
}