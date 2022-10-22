// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "GAS/Attributes/PELevelingAS.h"
#include "GAS/Attributes/PEBasicStatusAS.h"
#include "GAS/Attributes/PECustomStatusAS.h"
#include "GameplayEffectExtension.h"
#include "GameplayEffectTypes.h"
#include "AbilitySystemComponent.h"
#include "Runtime/Engine/Public/Net/UnrealNetwork.h"

UPELevelingAS::UPELevelingAS(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), CurrentLevel(0.f), CurrentExperience(0.f), RequiredExperience(1.f)
{
	static const ConstructorHelpers::FObjectFinder<UDataTable> LevelingAttributesMetaData_ObjRef(TEXT("/Game/Main/Data/GAS/AttributeMetaDatas/DT_LevelingAS"));
	if (LevelingAttributesMetaData_ObjRef.Succeeded())
	{
		UAttributeSet::InitFromMetaDataTable(LevelingAttributesMetaData_ObjRef.Object);
	}

	static const ConstructorHelpers::FObjectFinder<UDataTable> LevelingBonus_ObjRef(TEXT("/Game/Main/Data/GAS/AttributeMetaDatas/DT_LevelingBonus"));
	if (LevelingBonus_ObjRef.Succeeded())
	{
		LevelingBonusData = LevelingBonus_ObjRef.Object;
	}
}

void UPELevelingAS::PostAttributeChange(const FGameplayAttribute& Attribute, const float OldValue, const float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetCurrentExperienceAttribute() && NewValue >= GetRequiredExperience() && !LevelingBonusData.IsNull())
	{
		UAbilitySystemComponent* const AbilityComp = GetOwningAbilitySystemComponentChecked();

		// This block will verify if the leveling bonus table exists and modify
		// the attributes accordingly the current (new) level
		if (const UDataTable* const LevelingBonus_Table = LevelingBonusData.LoadSynchronous())
		{
			const FPELevelingData LevelingInfo = *LevelingBonus_Table->FindRow<FPELevelingData>(*FString::FromInt(GetCurrentLevel() + 1), FString());

			if constexpr (&LevelingInfo != nullptr)
			{
				AbilityComp->ApplyModToAttribute(UPEBasicStatusAS::GetMaxHealthAttribute(), EGameplayModOp::Additive, LevelingInfo.BonusMaxHealth);
				AbilityComp->ApplyModToAttribute(UPEBasicStatusAS::GetMaxStaminaAttribute(), EGameplayModOp::Additive, LevelingInfo.BonusMaxStamina);
				AbilityComp->ApplyModToAttribute(UPEBasicStatusAS::GetMaxManaAttribute(), EGameplayModOp::Additive, LevelingInfo.BonusMaxMana);

				AbilityComp->ApplyModToAttribute(UPECustomStatusAS::GetAttackRateAttribute(), EGameplayModOp::Additive, LevelingInfo.BonusAttackRate);
				AbilityComp->ApplyModToAttribute(UPECustomStatusAS::GetDefenseRateAttribute(), EGameplayModOp::Additive, LevelingInfo.BonusDefenseRate);

				const float NewExperience = GetCurrentExperience() - GetRequiredExperience();
				SetRequiredExperience(LevelingInfo.RequiredExp);
				SetCurrentLevel(GetCurrentLevel() + 1);
				SetCurrentExperience(NewExperience);
			}
		}
	}
}

#pragma region Attribute Replication
void UPELevelingAS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UPELevelingAS, CurrentLevel, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPELevelingAS, CurrentExperience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPELevelingAS, RequiredExperience, COND_None, REPNOTIFY_Always);
}

void UPELevelingAS::OnRep_CurrentLevel(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPELevelingAS, CurrentLevel, OldValue);
}

void UPELevelingAS::OnRep_CurrentExperience(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPELevelingAS, CurrentExperience, OldValue);
}

void UPELevelingAS::OnRep_RequiredExperience(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPELevelingAS, RequiredExperience, OldValue);
}
#pragma endregion Attribute Replication
