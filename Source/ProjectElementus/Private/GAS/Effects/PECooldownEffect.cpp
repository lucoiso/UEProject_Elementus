// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "GAS/Effects/PECooldownEffect.h"
#include "Management/Data/PEGlobalTags.h"

UPECooldownEffect::UPECooldownEffect(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;

	FSetByCallerFloat SetByCallerDuration;
	SetByCallerDuration.DataTag = GlobalTag_SetByCallerDuration;

	DurationMagnitude = FGameplayEffectModifierMagnitude(SetByCallerDuration);
}