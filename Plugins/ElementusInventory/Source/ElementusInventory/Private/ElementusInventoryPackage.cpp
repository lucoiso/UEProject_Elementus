// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "ElementusInventoryPackage.h"
#include "ElementusInventoryComponent.h"
#include "ElementusInventoryFunctions.h"

AElementusInventoryPackage::AElementusInventoryPackage()
{
	// The target dev will define if it should replicate or not
	bReplicates = false;

	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	PackageMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PackageMesh"));
	RootComponent = PackageMesh;

	PackageInventory = CreateDefaultSubobject<UElementusInventoryComponent>(TEXT("PackageInventory"));

	// Turn component replication on - will be not replicated if owning actor is set to not replicate.
	PackageInventory->SetIsReplicated(true);
}

void AElementusInventoryPackage::PutItemIntoPackage(FElementusItemInfo ItemInfo,
                                                    UElementusInventoryComponent* FromInventory) const
{
	if (int ItemIndex;
		UElementusInventoryFunctions::FindElementusItemInfoByDataInArr(ItemInfo.ItemData,
		                                                               FromInventory->ItemStack,
		                                                               ItemIndex))
	{
		ItemInfo.ItemQuantity =
			FMath::Clamp(ItemInfo.ItemQuantity, 0, FromInventory->ItemStack[ItemIndex].ItemQuantity);

		FromInventory->DiscardItemByData(ItemInfo.ItemData, ItemInfo.ItemQuantity);
		PackageInventory->AddItemByData(ItemInfo.ItemData, ItemInfo.ItemQuantity);
	}
}

void AElementusInventoryPackage::GetItemFromPackage(FElementusItemInfo ItemInfo,
                                                    UElementusInventoryComponent* ToInventory)
{
	if (int ItemIndex;
		UElementusInventoryFunctions::FindElementusItemInfoByDataInArr(ItemInfo.ItemData,
		                                                               PackageInventory->ItemStack,
		                                                               ItemIndex))
	{
		ItemInfo.ItemQuantity =
			FMath::Clamp(ItemInfo.ItemQuantity, 0, PackageInventory->ItemStack[ItemIndex].ItemQuantity);

		PackageInventory->DiscardItemByData(ItemInfo.ItemData, ItemInfo.ItemQuantity);
		ToInventory->AddItemByData(ItemInfo.ItemData, ItemInfo.ItemQuantity);

		if (PackageInventory->ItemStack.IsEmpty())
		{
			Destroy();
		}
	}
}
