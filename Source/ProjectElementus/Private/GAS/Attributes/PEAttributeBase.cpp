// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "GAS/Attributes/PEAttributeBase.h"
#include "GAS/System/PEAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PEAttributeBase)

UPEAttributeBase::UPEAttributeBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{	
}

void UPEAttributeBase::AdjustAttributeForMaxChange(const FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, const float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty) const
{
	if (UAbilitySystemComponent* const AbilityComp = GetOwningAbilitySystemComponent())
	{
		if (const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
			!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilityComp)
		{
			const float CurrentValue = AffectedAttribute.GetCurrentValue();
			const float NewDelta = CurrentMaxValue > 0.f ? CurrentValue * NewMaxValue / CurrentMaxValue - CurrentValue : NewMaxValue;

			AbilityComp->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
		}
	}
}

void UPEAttributeBase::InitFromMetaDataTable(const UDataTable* DataTable)
{
	Super::InitFromMetaDataTable(DataTable);

	if (UPEAbilitySystemComponent* const AbilityComp = GetCastedAbilitySystemComponent<UPEAbilitySystemComponent>())
	{
		AbilityComp->InitializeAttributeViewModel(this);
	}
}
