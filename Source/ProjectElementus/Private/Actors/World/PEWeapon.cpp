// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Actors/World/PEWeapon.h"
#include "Actors/Character/PECharacter.h"

UPEWeapon::UPEWeapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UPEWeapon::ProcessEquipmentApplication(APECharacter* EquipmentOwner)
{
	check(!WeaponMesh.IsNull());

	USkeletalMeshComponent* InMesh = NewObject<USkeletalMeshComponent>(EquipmentOwner);
	InMesh->SetSkeletalMesh(WeaponMesh.LoadSynchronous());
	InMesh->ComponentTags.Add(*FString::Printf(TEXT("ElementusEquipment_%s"), *WeaponMesh->GetName()));
	UE_LOG(LogTemp, Warning, TEXT("Tag 1: %s"),
	       *FString::Printf(TEXT("ElementusEquipment_%s"), *WeaponMesh->GetName()));

	EquipmentOwner->AddOwnedComponent(InMesh);

	InMesh->AttachToComponent(EquipmentOwner->GetMesh(),
	                          FAttachmentTransformRules::SnapToTargetNotIncludingScale,
	                          SocketToAttach);

	EquipmentOwner->FinishAndRegisterComponent(InMesh);

	Super::ProcessEquipmentApplication(EquipmentOwner);
}

void UPEWeapon::ProcessEquipmentRemoval(APECharacter* EquipmentOwner)
{
	check(!WeaponMesh.IsNull());

	const TArray<UActorComponent*> CompArr =
		EquipmentOwner->GetComponentsByTag(USkeletalMeshComponent::StaticClass(),
		                                   *FString::Printf(TEXT("ElementusEquipment_%s"), *WeaponMesh->GetName()));

	UE_LOG(LogTemp, Warning, TEXT("CompArr.Num(): %d"), CompArr.Num());
	UE_LOG(LogTemp, Warning, TEXT("Tag 2: %s"),
	       *FString::Printf(TEXT("ElementusEquipment_%s"), *WeaponMesh->GetName()));

	for (const auto& Iterator : CompArr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Removing Component: %s"), *Iterator->GetName());

		Iterator->UnregisterComponent();
		Iterator->RemoveFromRoot();
		Iterator->DestroyComponent();
	}

	Super::ProcessEquipmentRemoval(EquipmentOwner);
}
