// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Actors/Interfaces/PEEquipment.h"
#include "Actors/Character/PECharacter.h"
#include "GAS/System/PEAbilitySystemComponent.h"

void UPEEquipment::ProcessEquipmentApplication(APECharacter* EquipmentOwner)
{
	if (UPEAbilitySystemComponent* TargetABSC =
		Cast<UPEAbilitySystemComponent>(EquipmentOwner->GetAbilitySystemComponent()))
	{
		for (const auto& Effect : EquipmentEffects)
		{
			TargetABSC->ApplyEffectGroupedDataToSelf(Effect);
		}
	}
}

void UPEEquipment::ProcessEquipmentRemoval(APECharacter* EquipmentOwner)
{
	for (const auto& Effect : EquipmentEffects)
	{
		EquipmentOwner->GetAbilitySystemComponent()->RemoveActiveGameplayEffectBySourceEffect(
			Effect.EffectClass,
			EquipmentOwner->GetAbilitySystemComponent(),
			1
		);
	}
}
