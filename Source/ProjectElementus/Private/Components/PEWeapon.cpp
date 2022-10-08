// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Components/PEWeapon.h"
#include "AbilitySystemComponent.h"
#include "Actors/Character/PECharacter.h"
#include "Management/Data/PEGlobalTags.h"

UPEWeapon::UPEWeapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

bool UPEWeapon::ProcessEquipmentApplication(APECharacter* EquipmentOwner)
{
	check(!WeaponMesh.IsNull());

	if (!IsValid(EquipmentOwner))
	{
		return false;
	}

	if (!EquipmentOwner->HasAuthority())
	{
		return false;
	}

	EquipmentOwner->GetAbilitySystemComponent()->AddLooseGameplayTag(GlobalTag_WeaponSlot_Base);

	USkeletalMeshComponent* const InMesh = NewObject<USkeletalMeshComponent>(EquipmentOwner);

	if (!IsValid(InMesh))
	{
		return false;
	}

	InMesh->SetSkeletalMesh(WeaponMesh.LoadSynchronous());
	InMesh->ComponentTags.Add(*FString::Printf(TEXT("ElementusEquipment_%s"), *WeaponMesh->GetName()));

	EquipmentOwner->AddOwnedComponent(InMesh);

	if (!InMesh->AttachToComponent(EquipmentOwner->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketToAttach))
	{
		return false;
	}

	EquipmentOwner->FinishAndRegisterComponent(InMesh);

	return Super::ProcessEquipmentApplication(EquipmentOwner);
}

bool UPEWeapon::ProcessEquipmentRemoval(APECharacter* EquipmentOwner)
{
	check(!WeaponMesh.IsNull());
	
	if (!IsValid(EquipmentOwner))
	{
		return false;
	}

	if (!EquipmentOwner->HasAuthority())
	{
		return false;
	}
	
	EquipmentOwner->GetAbilitySystemComponent()->RemoveLooseGameplayTag(GlobalTag_WeaponSlot_Base);

	const TArray<UActorComponent*> CompArr = EquipmentOwner->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), *FString::Printf(TEXT("ElementusEquipment_%s"), *WeaponMesh->GetName()));
	
	if (CompArr.IsEmpty())
	{
		return false;
	}

	for (UActorComponent* const& Iterator : CompArr)
	{
		Iterator->UnregisterComponent();
		Iterator->RemoveFromRoot();
		Iterator->DestroyComponent();
	}

	return Super::ProcessEquipmentRemoval(EquipmentOwner);
}
