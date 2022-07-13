// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "ElementusInventoryPackage.h"
#include "ElementusInventoryComponent.h"
#include "ElementusInventoryFunctions.h"

AElementusInventoryPackage::AElementusInventoryPackage()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	PackageMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PackageMesh"));
	PackageMesh->SetupAttachment(RootComponent);

	PackageInventory = CreateDefaultSubobject<UElementusInventoryComponent>(TEXT("PackageInventory"));
	PackageInventory->SetIsReplicated(true);
}

void AElementusInventoryPackage::PutItemIntoPackage(TMap<FPrimaryAssetId, int32>& ItemInfo,
                                                    UElementusInventoryComponent* FromInventory) const
{
	UElementusInventoryFunctions::TradeElementusItem(ItemInfo, FromInventory, PackageInventory);
}

void AElementusInventoryPackage::GetItemFromPackage(TMap<FPrimaryAssetId, int32>& ItemInfo,
                                                    UElementusInventoryComponent* ToInventory) const
{
	UElementusInventoryFunctions::TradeElementusItem(ItemInfo, PackageInventory, ToInventory);
}
