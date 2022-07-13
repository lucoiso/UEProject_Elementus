// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "ElementusInventoryData.h"
#include "Components/ActorComponent.h"
#include "ElementusInventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FElementusInventoryUpdate, const FPrimaryAssetId, ItemId, const int32,
                                             NewQuantity);

UCLASS(Blueprintable, ClassGroup=(Custom), meta =(BlueprintSpawnableComponent),
	Category = "Project Elementus | Classes")
class ELEMENTUSINVENTORY_API UElementusInventoryComponent final : public UActorComponent
{
	GENERATED_BODY()

public:
	UElementusInventoryComponent();

	float GetCurrentWeight() const;

	UPROPERTY(BlueprintReadWrite, Category = "Elementus Inventory")
	float MaxWeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elementus Inventory",
		meta = (TitleProperty = "{Item Data} -> {Quantity}", AllowedClasses = "ElementusItemData"))
	TMap<FPrimaryAssetId, int32> ItemStack;

	UPROPERTY(BlueprintAssignable, Category = "Elementus Inventory")
	FElementusInventoryUpdate OnInventoryUpdate;

	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	void AddElementusItem(const FPrimaryAssetId& ItemId, const int32 Quantity);

	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	void DiscardElementusItem(const FPrimaryAssetId& ItemId, const int32 Quantity);

	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	void DebugInventoryStack();

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION(NetMulticast, Reliable)
	void AddElementusItem_Internal(const FPrimaryAssetId& ItemId,
	                               const int32 Quantity);

	UFUNCTION(NetMulticast, Reliable)
	void DiscardElementusItem_Internal(const FPrimaryAssetId& ItemId,
	                                   const int32 Quantity);

	void NotifyInventoryChange(const FPrimaryAssetId& ItemId, const int32 Quantity);

	void UpdateCurrentWeight();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Elementus Inventory",
		meta = (AllowPrivateAccess = "true"))
	float CurrentWeight;
};
