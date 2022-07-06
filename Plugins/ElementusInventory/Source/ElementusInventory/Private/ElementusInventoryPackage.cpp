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

void AElementusInventoryPackage::PutItemIntoPackage(const FElementusItemInfo ItemInfo,
                                                    UElementusInventoryComponent* FromInventory) const
{
	UElementusInventoryFunctions::TradeElementusItem(ItemInfo, FromInventory, PackageInventory);
}

void AElementusInventoryPackage::GetItemFromPackage(const FElementusItemInfo ItemInfo,
                                                    UElementusInventoryComponent* ToInventory) const
{
	UElementusInventoryFunctions::TradeElementusItem(ItemInfo, PackageInventory, ToInventory);
}
