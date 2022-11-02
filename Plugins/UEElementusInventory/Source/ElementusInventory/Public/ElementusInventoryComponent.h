// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEElementusInventory

#pragma once

#include "CoreMinimal.h"
#include "ElementusInventoryData.h"
#include "Components/ActorComponent.h"
#include "ElementusInventoryComponent.generated.h"

UENUM(Category = "Elementus Inventory | Enumerations")
enum class EElementusInventoryUpdateOperation : uint8
{
	None,
	Add,
	Remove
};

USTRUCT(Category = "Elementus Inventory | Structures")
struct FItemModifierData
{
	GENERATED_USTRUCT_BODY()

	FItemModifierData() = default;

	explicit FItemModifierData(const FElementusItemInfo& InItemInfo) : ItemInfo(InItemInfo)
	{
	}

	explicit FItemModifierData(const FElementusItemInfo& InItemInfo, const int32& InIndex) : ItemInfo(InItemInfo), Index(InIndex)
	{
	}

	FElementusItemInfo ItemInfo = FElementusItemInfo();
	int32 Index = INDEX_NONE;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FElementusInventoryUpdate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FElementusInventoryEmpty);

UCLASS(Blueprintable, ClassGroup=(Custom), Category = "Elementus Inventory | Classes", EditInlineNew, meta = (BlueprintSpawnableComponent))
class ELEMENTUSINVENTORY_API UElementusInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	explicit UElementusInventoryComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	/* Get the current inventory weight */
	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	float GetCurrentWeight() const;

	/* Get the current num of items in this inventory */
	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	int32 GetCurrentNumItems() const;

	UPROPERTY(BlueprintAssignable, Category = "Elementus Inventory")
	FElementusInventoryUpdate OnInventoryUpdate;

	UPROPERTY(BlueprintAssignable, Category = "Elementus Inventory")
	FElementusInventoryEmpty OnInventoryEmpty;

	/* Get the items that this inventory have */
	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	TArray<FElementusItemInfo> GetItemsArray() const;

	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	FElementusItemInfo& GetItemReferenceAt(const int32 Index);

	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	FElementusItemInfo GetItemCopyAt(const int32 Index) const;

	/* Check if this inventory can receive the passed item */
	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	virtual bool CanReceiveItem(const FElementusItemInfo InItemInfo) const;

	/* Check if this inventory can give the passed item */
	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	virtual bool CanGiveItem(const FElementusItemInfo InItemInfo) const;

	/* Find the first elementus item that matches the specified info */
	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	bool FindFirstItemIndexWithInfo(const FElementusItemInfo InItemInfo, int32& OutIndex, const FGameplayTagContainer IgnoreTags = FGameplayTagContainer(), const int32 Offset = 0) const;

	/* Find the first elementus item that matches the specified tag container */
	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	bool FindFirstItemIndexWithTags(const FGameplayTagContainer WithTags, int32& OutIndex, const FGameplayTagContainer IgnoreTags = FGameplayTagContainer(), const int32 Offset = 0) const;

	/* Find the first elementus item that matches the specified id */
	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	bool FindFirstItemIndexWithId(const FPrimaryElementusItemId InId, int32& OutIndex, const FGameplayTagContainer IgnoreTags = FGameplayTagContainer(), const int32 Offset = 0) const;


	/* Find the first elementus item that matches the specified info */
	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	bool FindAllItemIndexesWithInfo(const FElementusItemInfo InItemInfo, TArray<int32>& OutIndexes, const FGameplayTagContainer IgnoreTags = FGameplayTagContainer()) const;

	/* Find the first elementus item that matches the specified tag container */
	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	bool FindAllItemIndexesWithTags(const FGameplayTagContainer WithTags, TArray<int32>& OutIndexes, const FGameplayTagContainer IgnoreTags = FGameplayTagContainer()) const;

	/* Find the first elementus item that matches the specified id */
	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	bool FindAllItemIndexesWithId(const FPrimaryElementusItemId InId, TArray<int32>& OutIndexes, const FGameplayTagContainer IgnoreTags = FGameplayTagContainer()) const;

	/* Check if the inventory stack contains a item that matches the specified info */
	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	bool ContainsItem(const FElementusItemInfo InItemInfo) const;

	/* Print debug informations in the log about this inventory */
	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	virtual void DebugInventory();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Elementus Inventory")
	void ClearInventory();

	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "Elementus Inventory")
	void UpdateWeight();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Elementus Inventory")
	void GetItemsFrom(UElementusInventoryComponent* OtherInventory, const TArray<int32>& ItemIndexes);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Elementus Inventory")
	void GiveItemsTo(UElementusInventoryComponent* OtherInventory, const TArray<int32>& ItemIndexes);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Elementus Inventory")
	void DiscardItems(const TArray<int32>& ItemIndexes);

protected:
	/* Items that this inventory have */
	UPROPERTY(ReplicatedUsing=NotifyInventoryChange, EditAnywhere, BlueprintReadOnly, Category = "Elementus Inventory", meta = (Getter = "GetItemsArray", ArrayClamp = "MaxNumItems"))
	TArray<FElementusItemInfo> ElementusItems;

	/* Current weight of this inventory */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Elementus Inventory", meta = (AllowPrivateAccess = "true"))
	float CurrentWeight;

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void RefreshInventory();

private:
	/* Max weight allowed for this inventory */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Elementus Inventory", meta=(AllowPrivateAccess = "true", ClampMin = "0", UIMin = "0"))
	float MaxWeight;

	/* Max num of items allowed for this inventory */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Elementus Inventory", meta=(AllowPrivateAccess = "true", ClampMax = "100", UIMax = "100", ClampMin = "1", UIMin = "1"))
	int32 MaxNumItems;

	void ForceWeightUpdate();
	void ForceInventoryValidation();

public:
	/* Add a item to this inventory */
	void UpdateElementusItems(const TArray<FElementusItemInfo>& Modifiers, const EElementusInventoryUpdateOperation Operation);

private:
	UFUNCTION(Server, Reliable)
	void Server_ProcessInventoryAddition_Internal(const TArray<FItemModifierData>& Modifiers);

	UFUNCTION(Server, Reliable)
	void Server_ProcessInventoryRemoval_Internal(const TArray<FItemModifierData>& Modifiers);

	UFUNCTION(Category = "Elementus Inventory")
	void NotifyInventoryChange();
};
