// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "GAS/Attributes/PELevelingAS.h"
#include "GAS/Attributes/PEBasicStatusAS.h"
#include "GAS/Attributes/PECustomStatusAS.h"

#include "GAS/System/PEAbilitySystemGlobals.h"

#include "GameplayEffectExtension.h"
#include "GameplayEffectTypes.h"

#include "AbilitySystemComponent.h"
#include "Runtime/Engine/Public/Net/UnrealNetwork.h"

UPELevelingAS::UPELevelingAS(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, CurrentLevel(1.f)
	, CurrentExperience(1.f)
	, RequiredExperience(1.f)
{	
	InitFromMetaDataTable(UPEAbilitySystemGlobals::Get().GetLevelingAttributeMetaData());
}

void UPELevelingAS::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetCurrentExperienceAttribute() && NewValue >= GetRequiredExperience())
	{
		UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
		const UDataTable* LevelingBonus_Table = UPEAbilitySystemGlobals::Get().GetLevelingBonusData();
			
		if (ensureMsgf(IsValid(AbilityComp) && IsValid(LevelingBonus_Table), TEXT("%s have a invalid Parameters"), *GetName()))
		{
			const FPELevelingData LevelingInfo = *LevelingBonus_Table->FindRow<FPELevelingData>(
				FName(*FString::FromInt(GetCurrentLevel() + 1)), "");

#if __cplusplus > 201402L // Check if C++ > C++14
			if constexpr (&LevelingInfo != nullptr)
#else
			if (&LevelingInfo != nullptr)
#endif
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