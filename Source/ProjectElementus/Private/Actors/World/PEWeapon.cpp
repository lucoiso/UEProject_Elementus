// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Actors/World/PEWeapon.h"
#include "AbilitySystemComponent.h"
#include "Actors/Character/PECharacter.h"
#include "Management/Data/PEGlobalTags.h"

UPEWeapon::UPEWeapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UPEWeapon::ProcessEquipmentApplication(APECharacter* EquipmentOwner)
{
	check(!WeaponMesh.IsNull());

	EquipmentOwner->GetAbilitySystemComponent()->AddLooseGameplayTag(GlobalTag_WeaponEquipped);

	USkeletalMeshComponent* const InMesh = NewObject<USkeletalMeshComponent>(EquipmentOwner);
	InMesh->SetSkeletalMesh(WeaponMesh.LoadSynchronous());
	InMesh->ComponentTags.Add(*FString::Printf(TEXT("ElementusEquipment_%s"), *WeaponMesh->GetName()));

	EquipmentOwner->AddOwnedComponent(InMesh);

	InMesh->AttachToComponent(EquipmentOwner->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketToAttach);

	EquipmentOwner->FinishAndRegisterComponent(InMesh);

	Super::ProcessEquipmentApplication(EquipmentOwner);
}

void UPEWeapon::ProcessEquipmentRemoval(APECharacter* EquipmentOwner)
{
	EquipmentOwner->GetAbilitySystemComponent()->RemoveLooseGameplayTag(GlobalTag_WeaponEquipped);

	check(!WeaponMesh.IsNull());

	const TArray<UActorComponent*> CompArr = EquipmentOwner->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), *FString::Printf(TEXT("ElementusEquipment_%s"), *WeaponMesh->GetName()));

	for (UActorComponent* const& Iterator : CompArr)
	{
		Iterator->UnregisterComponent();
		Iterator->RemoveFromRoot();
		Iterator->DestroyComponent();
	}

	Super::ProcessEquipmentRemoval(EquipmentOwner);
}
