// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "GAS/System/PEAbilitySystemComponent.h"
#include "GAS/System/PEAbilityData.h"
#include "GAS/System/PEEffectData.h"
#include "ViewModels/Attributes/PEVM_AttributeBasic.h"
#include "ViewModels/Attributes/PEVM_AttributeCustom.h"
#include "ViewModels/Attributes/PEVM_AttributeLeveling.h"
#include "GAS/Attributes/PEBasicStatusAS.h"
#include "GAS/Attributes/PECustomStatusAS.h"
#include "GAS/Attributes/PELevelingAS.h"

UPEAbilitySystemComponent::UPEAbilitySystemComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SetIsReplicated(true);
	ReplicationMode = EGameplayEffectReplicationMode::Minimal;

	BasicAttributes_VM = CreateDefaultSubobject<UPEVM_AttributeBasic>(TEXT("BasicAttributes_ViewModel"));
	CustomAttributes_VM = CreateDefaultSubobject<UPEVM_AttributeCustom>(TEXT("CustomAttributes_ViewModel"));
	LevelingAttributes_VM = CreateDefaultSubobject<UPEVM_AttributeLeveling>(TEXT("LevelingAttributes_ViewModel"));
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

#define REGISTER_ATTRIBUTE_DELEGATE_TO_VIEWMODEL(AttributeClass, AttributeName, ViewModelClass, ViewModelObject) \
	GetGameplayAttributeValueChangeDelegate(##AttributeClass##::Get##AttributeName##Attribute()).AddUObject(ViewModelObject, &##ViewModelClass##::OnAttributeChange)

void UPEAbilitySystemComponent::InitializeComponent()
{
	Super::InitializeComponent();

#pragma region Basic Status
	REGISTER_ATTRIBUTE_DELEGATE_TO_VIEWMODEL(UPEBasicStatusAS, Health, UPEVM_AttributeBasic, BasicAttributes_VM);
	REGISTER_ATTRIBUTE_DELEGATE_TO_VIEWMODEL(UPEBasicStatusAS, MaxHealth, UPEVM_AttributeBasic, BasicAttributes_VM);
	REGISTER_ATTRIBUTE_DELEGATE_TO_VIEWMODEL(UPEBasicStatusAS, Stamina, UPEVM_AttributeBasic, BasicAttributes_VM);
	REGISTER_ATTRIBUTE_DELEGATE_TO_VIEWMODEL(UPEBasicStatusAS, MaxStamina, UPEVM_AttributeBasic, BasicAttributes_VM);
	REGISTER_ATTRIBUTE_DELEGATE_TO_VIEWMODEL(UPEBasicStatusAS, Mana, UPEVM_AttributeBasic, BasicAttributes_VM);
	REGISTER_ATTRIBUTE_DELEGATE_TO_VIEWMODEL(UPEBasicStatusAS, MaxMana, UPEVM_AttributeBasic, BasicAttributes_VM);
#pragma endregion Basic Status

#pragma region Custom Status
	REGISTER_ATTRIBUTE_DELEGATE_TO_VIEWMODEL(UPECustomStatusAS, AttackRate, UPEVM_AttributeCustom, CustomAttributes_VM);
	REGISTER_ATTRIBUTE_DELEGATE_TO_VIEWMODEL(UPECustomStatusAS, DefenseRate, UPEVM_AttributeCustom, CustomAttributes_VM);
	REGISTER_ATTRIBUTE_DELEGATE_TO_VIEWMODEL(UPECustomStatusAS, SpeedRate, UPEVM_AttributeCustom, CustomAttributes_VM);
	REGISTER_ATTRIBUTE_DELEGATE_TO_VIEWMODEL(UPECustomStatusAS, JumpRate, UPEVM_AttributeCustom, CustomAttributes_VM);
	REGISTER_ATTRIBUTE_DELEGATE_TO_VIEWMODEL(UPECustomStatusAS, Gold, UPEVM_AttributeCustom, CustomAttributes_VM);
#pragma endregion Custom Status

#pragma region Leveling Status
	REGISTER_ATTRIBUTE_DELEGATE_TO_VIEWMODEL(UPELevelingAS, CurrentLevel, UPEVM_AttributeLeveling, LevelingAttributes_VM);
	REGISTER_ATTRIBUTE_DELEGATE_TO_VIEWMODEL(UPELevelingAS, CurrentExperience, UPEVM_AttributeLeveling, LevelingAttributes_VM);
	REGISTER_ATTRIBUTE_DELEGATE_TO_VIEWMODEL(UPELevelingAS, RequiredExperience, UPEVM_AttributeLeveling, LevelingAttributes_VM);
#pragma endregion Leveling Status
}
#undef REGISTER_ATTRIBUTE_DELEGATE_TO_VIEWMODEL