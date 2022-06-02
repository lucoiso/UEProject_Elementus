// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "GAS/Attributes/PEBasicStatusAS.h"
#include "GameplayEffectExtension.h"
#include "GameplayEffectTypes.h"
#include "AbilitySystemComponent.h"
#include "Runtime/Engine/Public/Net/UnrealNetwork.h"

UPEBasicStatusAS::UPEBasicStatusAS(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	  , Health(500.f)
	  , MaxHealth(500.f)
	  , Stamina(250.f)
	  , MaxStamina(250.f)
	  , Mana(100.f)
	  , MaxMana(100.f)
{
	static const ConstructorHelpers::FObjectFinder<UDataTable> MainAttributesMetaData_ObjRef(
		TEXT("/Game/Main/GAS/Data/DT_BasicStatusAS"));
	if constexpr (&MainAttributesMetaData_ObjRef.Object != nullptr)
	{
		UAttributeSet::InitFromMetaDataTable(MainAttributesMetaData_ObjRef.Object);
	}

	static const ConstructorHelpers::FClassFinder<UGameplayEffect> DeathGameplayEffect_ClassRef(
		TEXT("/Game/Main/GAS/Effects/States/GE_Death"));
	if constexpr (&DeathGameplayEffect_ClassRef.Class != nullptr)
	{
		GlobalDeathEffect = DeathGameplayEffect_ClassRef.Class;
	}
}

void UPEBasicStatusAS::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetMaxHealthAttribute())
	{
		AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
	}

	else if (Attribute == GetMaxManaAttribute())
	{
		AdjustAttributeForMaxChange(Mana, MaxMana, NewValue, GetManaAttribute());
	}

	else if (Attribute == GetMaxStaminaAttribute())
	{
		AdjustAttributeForMaxChange(Stamina, MaxStamina, NewValue, GetStaminaAttribute());
	}
}

void UPEBasicStatusAS::PostAttributeChange(const FGameplayAttribute& Attribute, const float OldValue,
                                           const float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (NewValue <= 0.f)
	{
		if (Attribute == GetHealthAttribute())
		{
			GetOwningAbilitySystemComponentChecked()->CancelAllAbilities();

			if (!GlobalDeathEffect.IsNull())
			{
				GetOwningAbilitySystemComponent()->ApplyGameplayEffectToSelf(
					GlobalDeathEffect.LoadSynchronous()->GetDefaultObject<UGameplayEffect>(), 1.f,
					GetOwningAbilitySystemComponent()->MakeEffectContext());
			}
		}

		if (Attribute == GetStaminaAttribute())
		{
			const FGameplayTagContainer StaminaCostTagContainer
			{
				FGameplayTag::RequestGameplayTag(FName("GameplayAbility.State.CostWhileActive.Stamina"))
			};

			GetOwningAbilitySystemComponentChecked()->CancelAbilities(&StaminaCostTagContainer);
		}

		if (Attribute == GetManaAttribute())
		{
			const FGameplayTagContainer ManaCostTagContainer
			{
				FGameplayTag::RequestGameplayTag(FName("GameplayAbility.State.CostWhileActive.Mana"))
			};

			GetOwningAbilitySystemComponentChecked()->CancelAbilities(&ManaCostTagContainer);
		}
	}
}

void UPEBasicStatusAS::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		const float DamageDone = GetDamage();
		SetDamage(0.f);

		if (DamageDone > 0.f)
		{
			const float NewHealth = GetHealth() - DamageDone;

			SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));
		}
	}

	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}

	else if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}

	else if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), 0.f, GetMaxStamina()));
	}
}

void UPEBasicStatusAS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UPEBasicStatusAS, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPEBasicStatusAS, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPEBasicStatusAS, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPEBasicStatusAS, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPEBasicStatusAS, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPEBasicStatusAS, MaxStamina, COND_None, REPNOTIFY_Always);
}

void UPEBasicStatusAS::AdjustAttributeForMaxChange(const FGameplayAttributeData& AffectedAttribute,
                                                   const FGameplayAttributeData& MaxAttribute, const float NewMaxValue,
                                                   const FGameplayAttribute& AffectedAttributeProperty) const
{
	if (UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent())
	{
		if (const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
			!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilityComp)
		{
			const float CurrentValue = AffectedAttribute.GetCurrentValue();
			const float NewDelta = CurrentMaxValue > 0.f
				                       ? CurrentValue * NewMaxValue / CurrentMaxValue - CurrentValue
				                       : NewMaxValue;

			AbilityComp->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
		}
	}
}

void UPEBasicStatusAS::OnRep_Health(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPEBasicStatusAS, Health, OldValue);
}

void UPEBasicStatusAS::OnRep_MaxHealth(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPEBasicStatusAS, MaxHealth, OldValue);
}

void UPEBasicStatusAS::OnRep_Mana(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPEBasicStatusAS, Mana, OldValue);
}

void UPEBasicStatusAS::OnRep_MaxMana(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPEBasicStatusAS, MaxMana, OldValue);
}

void UPEBasicStatusAS::OnRep_Stamina(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPEBasicStatusAS, Stamina, OldValue);
}

void UPEBasicStatusAS::OnRep_MaxStamina(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPEBasicStatusAS, MaxStamina, OldValue);
}
