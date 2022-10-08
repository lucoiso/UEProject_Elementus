// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Actors/Interfaces/PEEquipment.h"
#include "Actors/Character/PECharacter.h"
#include "GAS/System/PEAbilitySystemComponent.h"

bool UPEEquipment::ProcessEquipmentApplication(APECharacter* EquipmentOwner)
{
	if (!IsValid(EquipmentOwner))
	{
		return false;
	}

	if (!EquipmentOwner->HasAuthority())
	{
		return false;
	}

	if (UPEAbilitySystemComponent* const TargetABSC = Cast<UPEAbilitySystemComponent>(EquipmentOwner->GetAbilitySystemComponent()))
	{
		for (const FGameplayEffectGroupedData& Effect : EquipmentEffects)
		{
			TargetABSC->ApplyEffectGroupedDataToSelf(Effect);
		}
		
		return true;
	}

	return false;
}

bool UPEEquipment::ProcessEquipmentRemoval(APECharacter* EquipmentOwner)
{
	if (!IsValid(EquipmentOwner))
	{
		return false;
	}

	if (!EquipmentOwner->HasAuthority())
	{
		return false;
	}
	
	if (UPEAbilitySystemComponent* const TargetABSC = Cast<UPEAbilitySystemComponent>(EquipmentOwner->GetAbilitySystemComponent()))
	{
		for (const FGameplayEffectGroupedData& Effect : EquipmentEffects)
		{
			TargetABSC->RemoveEffectGroupedDataFromSelf(Effect, EquipmentOwner->GetAbilitySystemComponent(), 1);
		}

		return true;
	}

	return false;
}
