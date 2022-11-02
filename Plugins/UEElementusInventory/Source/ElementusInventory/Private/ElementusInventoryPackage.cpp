// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEElementusInventory

#include "ElementusInventoryPackage.h"
#include "ElementusInventoryComponent.h"
#include "ElementusInventoryFunctions.h"
#include "Net/UnrealNetwork.h"

AElementusInventoryPackage::AElementusInventoryPackage(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), bDestroyWhenInventoryIsEmpty(false)
{
	bNetStartup = false;
	bNetLoadOnClient = false;
	bReplicates = true;

	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	PackageInventory = CreateDefaultSubobject<UElementusInventoryComponent>(TEXT("PackageInventory"));
	PackageInventory->SetIsReplicated(true);
}

void AElementusInventoryPackage::BeginPlay()
{
	Super::BeginPlay();

	SetDestroyOnEmpty(bDestroyWhenInventoryIsEmpty);

	if (bDestroyWhenInventoryIsEmpty && PackageInventory->GetItemsArray().IsEmpty())
	{
		Destroy();
	}
}

void AElementusInventoryPackage::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AElementusInventoryPackage, PackageInventory);
}

void AElementusInventoryPackage::PutItemIntoPackage(const TArray<FElementusItemInfo> ItemInfo, UElementusInventoryComponent* FromInventory)
{
	UElementusInventoryFunctions::TradeElementusItem(ItemInfo, FromInventory, PackageInventory);
}

void AElementusInventoryPackage::GetItemFromPackage(const TArray<FElementusItemInfo> ItemInfo, UElementusInventoryComponent* ToInventory)
{
	UElementusInventoryFunctions::TradeElementusItem(ItemInfo, PackageInventory, ToInventory);
}

void AElementusInventoryPackage::SetDestroyOnEmpty(const bool bDestroy)
{
	if (bDestroyWhenInventoryIsEmpty == bDestroy)
	{
		return;
	}

	bDestroyWhenInventoryIsEmpty = bDestroy;
	FElementusInventoryEmpty& Delegate = PackageInventory->OnInventoryEmpty;

	if (const bool bIsAlreadyBound = Delegate.IsAlreadyBound(this, &AElementusInventoryPackage::BeginPackageDestruction);
		bDestroy && !bIsAlreadyBound)
	{
		Delegate.AddDynamic(this, &AElementusInventoryPackage::BeginPackageDestruction);
	}
	else if (!bDestroy && bIsAlreadyBound)
	{
		Delegate.RemoveDynamic(this, &AElementusInventoryPackage::BeginPackageDestruction);
	}
}

bool AElementusInventoryPackage::GetDestroyOnEmpty() const
{
	return bDestroyWhenInventoryIsEmpty;
}

void AElementusInventoryPackage::BeginPackageDestruction_Implementation()
{
	// Check if this option is still active
	if (bDestroyWhenInventoryIsEmpty)
	{
		Destroy();
	}
	else
	{
		UE_LOG(LogElementusInventory, Warning, TEXT("ElementusInventory - %s: Package %s was not destroyed because the " "option 'bDestroyWhenInventoryIsEmpty' was disabled"), *FString(__func__), *GetName());
	}
}
