// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "ElementusInventoryData.h"
#include "Components/ActorComponent.h"
#include "ElementusInventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FElementusInventoryUpdate,
                                             const FPrimaryAssetId, ItemId,
                                             const int32, NewQuantity);

UCLASS(Blueprintable, ClassGroup=(Custom), meta =(BlueprintSpawnableComponent),
	Category = "Project Elementus | Classes")
class ELEMENTUSINVENTORY_API UElementusInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UElementusInventoryComponent();

	float GetCurrentWeight() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elementus Inventory")
	float MaxWeight;

	UPROPERTY(BlueprintAssignable, Category = "Elementus Inventory")
	FElementusInventoryUpdate OnInventoryUpdate;

	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	TMap<FPrimaryAssetId, int32> GetItemStack() const;

	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	void AddElementusItem(const FPrimaryAssetId& ItemId, const int32 Quantity);

	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	void DiscardElementusItem(const FPrimaryAssetId& ItemId, const int32 Quantity);

	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	virtual void DebugInventoryStack();

	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	virtual bool CanReceiveItem(const FPrimaryAssetId& ItemId, const int32 Quantity) const;

	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	virtual bool CanGiveItem(const FPrimaryAssetId& ItemId, const int32 Quantity) const;

protected:
	/* UInventoryItemData -> Quantity */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Elementus Inventory",
		meta = (Getter = "GetItemStack"))
	TMap<FPrimaryAssetId, int32> ItemStack;

	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
	UFUNCTION(NetMulticast, Reliable)
	void AddElementusItem_Internal(const FPrimaryAssetId& ItemId,
	                               const int32 Quantity);

	UFUNCTION(NetMulticast, Reliable)
	void DiscardElementusItem_Internal(const FPrimaryAssetId& ItemId,
	                                   const int32 Quantity);

	void NotifyInventoryChange(const FPrimaryAssetId& ItemId, const int32 Quantity);

	void UpdateCurrentWeight();
	void ValidateItemStack();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Elementus Inventory",
		meta = (AllowPrivateAccess = "true"))
	float CurrentWeight;
};
