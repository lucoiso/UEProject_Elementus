// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Actors/World/PEInventoryPackage.h"

#include "ElementusInventoryComponent.h"
#include "Actors/Character/PECharacter.h"
#include "Blueprint/UserWidget.h"

APEInventoryPackage::APEInventoryPackage(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	PackageMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PackageMesh"));
	PackageMesh->SetupAttachment(RootComponent);
	PackageMesh->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
	PackageMesh->SetRelativeScale3D(FVector(0.5f));

	static const ConstructorHelpers::FObjectFinder<UStaticMesh> PackageMesh_Obj(
		TEXT("/Game/Main/Meshes/LevelPrototyping/SM_ChamferCube"));
	if constexpr (&PackageMesh_Obj.Object != nullptr)
	{
		PackageMesh->SetStaticMesh(PackageMesh_Obj.Object);
	}

	static const ConstructorHelpers::FClassFinder<UUserWidget> TradeWidget_ClassRef(
		TEXT("/Game/Main/Blueprints/Widgets/Inventory/WB_Trade_w_Package"));
	if constexpr (&TradeWidget_ClassRef.Class != nullptr)
	{
		TradeWidgetClass = TradeWidget_ClassRef.Class;
	}

	PackageInventory->SetIsReplicated(true);
}

void APEInventoryPackage::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	PackageMesh->AddRelativeRotation(FRotator(0.f, DeltaSeconds * 15.f, 0.f));
}

bool APEInventoryPackage::IsInteractEnabled_Implementation() const
{
	return true;
}

void APEInventoryPackage::DoInteractionBehavior_Implementation(APECharacter* CharacterInteracting,
                                                               const FHitResult& HitResult)
{
	if (!CharacterInteracting->IsLocallyControlled())
	{
		return;
	}

	if (APlayerController* TargetController = CharacterInteracting->GetController<APlayerController>())
	{
		UClass* WidgetClass = TradeWidgetClass.LoadSynchronous();
		if (UUserWidget* TradeWidget = CreateWidget(TargetController, WidgetClass))
		{
			if (const FObjectProperty* PackageRef =
				FindFProperty<FObjectProperty>(WidgetClass, TEXT("PackageRef")))
			{
				PackageRef->SetPropertyValue_InContainer(TradeWidget, this);
			}
			TradeWidget->AddToViewport();
		}
	}
}
