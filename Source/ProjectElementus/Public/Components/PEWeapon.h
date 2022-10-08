// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "Actors/Interfaces/PEEquipment.h"
#include "PEWeapon.generated.h"

struct FGameplayEffectGroupedData;
class APECharacter;
class UPEGameplayAbility;
/**
 *
 */
UCLASS(Abstract, Blueprintable, Category = "Project Elementus | Classes")
class UPEWeapon : public UPEEquipment
{
	GENERATED_BODY()

	explicit UPEWeapon(const FObjectInitializer& ObjectInitializer);

public:
	UPROPERTY(EditAnywhere, Category = "Project Elementus | Properties")
	TArray<UPEGameplayAbility*> WeaponAbilities;

	UPROPERTY(EditAnywhere, Category = "Project Elementus | Properties")
	TSoftObjectPtr<USkeletalMesh> WeaponMesh;

	UPROPERTY(EditAnywhere, Category = "Project Elementus | Properties")
	FName SocketToAttach = NAME_None;

	virtual bool ProcessEquipmentApplication(APECharacter* EquipmentOwner) override;
	virtual bool ProcessEquipmentRemoval(APECharacter* EquipmentOwner) override;
};
