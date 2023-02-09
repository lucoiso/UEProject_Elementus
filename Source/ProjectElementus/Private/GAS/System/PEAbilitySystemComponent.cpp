// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "GAS/System/PEAbilitySystemComponent.h"
#include "GAS/System/PEAbilityData.h"
#include "GAS/System/PEEffectData.h"
#include "GAS/Attributes/PEBasicStatusAS.h"
#include "GAS/Attributes/PECustomStatusAS.h"
#include "GAS/Attributes/PELevelingAS.h"
#include "ViewModels/Attributes/PEVM_AttributeBasic.h"
#include "ViewModels/Attributes/PEVM_AttributeCustom.h"
#include "ViewModels/Attributes/PEVM_AttributeLeveling.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PEAbilitySystemComponent)

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

void UPEAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);
}

void UPEAbilitySystemComponent::InitializeAttributeViewModel(const UAttributeSet* AttributeSet)
{
	UE_LOG(LogTemp, Display, TEXT("%s - Initializing view model for attribute %s"), *FString(__func__), *AttributeSet->GetName());

	if (AttributeSet->GetClass()->IsChildOf<UPEBasicStatusAS>())
	{
		InitializeBasicAttributesViewModel(Cast<UPEBasicStatusAS>(AttributeSet));
	}
	else if (AttributeSet->GetClass()->IsChildOf<UPECustomStatusAS>())
	{
		InitializeCustomAttributesViewModel(Cast<UPECustomStatusAS>(AttributeSet));
	}
	else if (AttributeSet->GetClass()->IsChildOf<UPELevelingAS>())
	{
		InitializeLevelingAttributesViewModel(Cast<UPELevelingAS>(AttributeSet));
	}
}

#define REGISTER_ATTRIBUTE_DELEGATE(AttributeClass, AttributeName, ViewModelClass, ViewModelObject) \
if (GetGameplayAttributeValueChangeDelegate(##AttributeClass##::Get##AttributeName##Attribute()).IsBoundToObject(this)) \
{ \
	GetGameplayAttributeValueChangeDelegate(##AttributeClass##::Get##AttributeName##Attribute()).RemoveAll(this); \
} \
GetGameplayAttributeValueChangeDelegate(##AttributeClass##::Get##AttributeName##Attribute()).AddUObject(this, &UPEAbilitySystemComponent::OnViewModelAttributeChange); \
/* This block is used to initialize the first value of the attribute because the viewmodel is only updating when the attribute changes after the binding occurs */ \
{ \
	OnViewModelAttributeChange_Client(##AttributeClass##::Get##AttributeName##Attribute(), Attribute->Get##AttributeName##()); \
}

void UPEAbilitySystemComponent::InitializeBasicAttributesViewModel(const UPEBasicStatusAS* Attribute)
{
	REGISTER_ATTRIBUTE_DELEGATE(UPEBasicStatusAS, Health, UPEVM_AttributeBasic, BasicAttributes_VM);
	REGISTER_ATTRIBUTE_DELEGATE(UPEBasicStatusAS, MaxHealth, UPEVM_AttributeBasic, BasicAttributes_VM);
	REGISTER_ATTRIBUTE_DELEGATE(UPEBasicStatusAS, Stamina, UPEVM_AttributeBasic, BasicAttributes_VM);
	REGISTER_ATTRIBUTE_DELEGATE(UPEBasicStatusAS, MaxStamina, UPEVM_AttributeBasic, BasicAttributes_VM);
	REGISTER_ATTRIBUTE_DELEGATE(UPEBasicStatusAS, Mana, UPEVM_AttributeBasic, BasicAttributes_VM);
	REGISTER_ATTRIBUTE_DELEGATE(UPEBasicStatusAS, MaxMana, UPEVM_AttributeBasic, BasicAttributes_VM);
}

void UPEAbilitySystemComponent::InitializeCustomAttributesViewModel(const UPECustomStatusAS* Attribute)
{
	REGISTER_ATTRIBUTE_DELEGATE(UPECustomStatusAS, AttackRate, UPEVM_AttributeCustom, CustomAttributes_VM);
	REGISTER_ATTRIBUTE_DELEGATE(UPECustomStatusAS, DefenseRate, UPEVM_AttributeCustom, CustomAttributes_VM);
	REGISTER_ATTRIBUTE_DELEGATE(UPECustomStatusAS, SpeedRate, UPEVM_AttributeCustom, CustomAttributes_VM);
	REGISTER_ATTRIBUTE_DELEGATE(UPECustomStatusAS, JumpRate, UPEVM_AttributeCustom, CustomAttributes_VM);
	REGISTER_ATTRIBUTE_DELEGATE(UPECustomStatusAS, Gold, UPEVM_AttributeCustom, CustomAttributes_VM);
}

void UPEAbilitySystemComponent::InitializeLevelingAttributesViewModel(const UPELevelingAS* Attribute)
{
	REGISTER_ATTRIBUTE_DELEGATE(UPELevelingAS, CurrentLevel, UPEVM_AttributeLeveling, LevelingAttributes_VM);
	REGISTER_ATTRIBUTE_DELEGATE(UPELevelingAS, CurrentExperience, UPEVM_AttributeLeveling, LevelingAttributes_VM);
	REGISTER_ATTRIBUTE_DELEGATE(UPELevelingAS, RequiredExperience, UPEVM_AttributeLeveling, LevelingAttributes_VM);
}

#undef REGISTER_ATTRIBUTE_DELEGATE

void UPEAbilitySystemComponent::OnViewModelAttributeChange(const FOnAttributeChangeData& AttributeChangeData)
{
	OnViewModelAttributeChange_Client(AttributeChangeData.Attribute, AttributeChangeData.NewValue);
}

void UPEAbilitySystemComponent::OnViewModelAttributeChange_Client_Implementation(const FGameplayAttribute& Attribute, const float& NewValue)
{
	if (Attribute.GetAttributeSetClass()->IsChildOf<UPEBasicStatusAS>())
	{
		BasicAttributes_VM->NotifyAttributeChange(Attribute, NewValue);
	}
	else if (Attribute.GetAttributeSetClass()->IsChildOf<UPECustomStatusAS>())
	{
		CustomAttributes_VM->NotifyAttributeChange(Attribute, NewValue);
	}
	else if (Attribute.GetAttributeSetClass()->IsChildOf<UPELevelingAS>())
	{
		LevelingAttributes_VM->NotifyAttributeChange(Attribute, NewValue);
	}
}
