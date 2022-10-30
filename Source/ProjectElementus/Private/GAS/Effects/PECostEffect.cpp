// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "GAS/Effects/PECostEffect.h"
#include "GAS/Attributes/PEBasicStatusAS.h"
#include "Management/Data/PEGlobalTags.h"

UPECostEffect::UPECostEffect(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	
	FSetByCallerFloat SetByCallerDuration;
	SetByCallerDuration.DataTag = FGameplayTag::RequestGameplayTag(GlobalTag_SetByCallerDuration);
	
	/** From GameplayEffect.h: Duration in seconds. 0.0 for instantaneous effects; -1.0 for infinite duration. */
	DurationMagnitude = FGameplayEffectModifierMagnitude(SetByCallerDuration);

	Period = 0.333f;
	
	FSetByCallerFloat SetByCallerHealth;
	SetByCallerHealth.DataTag = FGameplayTag::RequestGameplayTag(GlobalTag_SetByCallerHealth);
	const FGameplayEffectModifierMagnitude HealthModifierMag(SetByCallerHealth);
	FGameplayModifierInfo HealthModifier;
	HealthModifier.Attribute = UPEBasicStatusAS::GetHealthAttribute();
	HealthModifier.ModifierMagnitude = HealthModifierMag;
	Modifiers.Add(HealthModifier);

	FSetByCallerFloat SetByCallerStamina;
	SetByCallerStamina.DataTag = FGameplayTag::RequestGameplayTag(GlobalTag_SetByCallerStamina);
	const FGameplayEffectModifierMagnitude StaminaModifierMag(SetByCallerStamina);
	FGameplayModifierInfo StaminaModifier;
	StaminaModifier.Attribute = UPEBasicStatusAS::GetStaminaAttribute();
	StaminaModifier.ModifierMagnitude = StaminaModifierMag;
	Modifiers.Add(StaminaModifier);

	FSetByCallerFloat SetByCallerMana;
	SetByCallerMana.DataTag = FGameplayTag::RequestGameplayTag(GlobalTag_SetByCallerMana);
	const FGameplayEffectModifierMagnitude ManaModifierMag(SetByCallerMana);
	FGameplayModifierInfo ManaModifier;
	ManaModifier.Attribute = UPEBasicStatusAS::GetManaAttribute();
	ManaModifier.ModifierMagnitude = ManaModifierMag;
	Modifiers.Add(ManaModifier);
}