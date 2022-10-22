// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "ElementusInventoryComponent.h"
#include "PEInventoryComponent.generated.h"

class UPEEquipment;
class APECharacter;
class UPEAbilitySystemComponent;

/**
 *
 */
UCLASS(Blueprintable, ClassGroup = (Custom), Category = "Project Elementus | Classes", EditInlineNew, meta = (BlueprintSpawnableComponent))
class PROJECTELEMENTUS_API UPEInventoryComponent : public UElementusInventoryComponent
{
	GENERATED_BODY()

public:
	explicit UPEInventoryComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual bool CanGiveItem(const FElementusItemInfo InItemInfo) const override;

	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	virtual bool EquipItem(const FElementusItemInfo& InItem);

	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	virtual bool UnnequipItem(FElementusItemInfo& InItem);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Project Elementus | Properties")
	TMap<FGameplayTag, FElementusItemInfo> EquipmentMap;

private:
	TWeakObjectPtr<UEnum> InputEnumHandle;

	void ProcessEquipmentAddition_Internal(APECharacter* OwningCharacter, UPEEquipment* Equipment);
	void ProcessEquipmentRemoval_Internal(APECharacter* OwningCharacter, UPEEquipment* Equipment);

	UFUNCTION(Server, Reliable)
	void AddEquipmentGASData_Server(UPEAbilitySystemComponent* TargetABSC, UPEEquipment* Equipment);
	
	UFUNCTION(Server, Reliable)
	void RemoveEquipmentGASData_Server(UPEAbilitySystemComponent* TargetABSC, UPEEquipment* Equipment);
	
	UFUNCTION(Server, Reliable)
	void ProcessEquipmentAttachment_Server(USkeletalMeshComponent* TargetMesh, UPEEquipment* Equipment);

	UFUNCTION(Server, Reliable)
	void ProcessEquipmentDettachment_Server(UPEEquipment* Equipment);

	UFUNCTION(NetMulticast, Reliable)
	void ProcessEquipmentAttachment_Multicast(USkeletalMeshComponent* TargetMesh, UPEEquipment* Equipment);
	
	UFUNCTION(NetMulticast, Reliable)
	void ProcessEquipmentDettachment_Multicast(UPEEquipment* Equipment);
};
