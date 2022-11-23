// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Actors/World/PEInventoryPackage.h"
#include "Actors/Character/PECharacter.h"
#include "Blueprint/UserWidget.h"
#include "Management/PEProjectSettings.h"

APEInventoryPackage::APEInventoryPackage(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	PackageMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PackageMesh"));
	PackageMesh->SetupAttachment(RootComponent);
	PackageMesh->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
	PackageMesh->SetRelativeScale3D(FVector(0.5f));
	PackageMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	PackageMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	static const ConstructorHelpers::FObjectFinder<UStaticMesh> PackageMesh_Obj(TEXT("/Game/Main/Meshes/LevelPrototyping/SM_ChamferCube"));
	if (PackageMesh_Obj.Succeeded())
	{
		PackageMesh->SetStaticMesh(PackageMesh_Obj.Object);
	}
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

void APEInventoryPackage::DoInteractionBehavior_Implementation(APECharacter* CharacterInteracting, const FHitResult& HitResult)
{
	if (!CharacterInteracting->IsLocallyControlled())
	{
		return;
	}

	if (APlayerController* const TargetController = CharacterInteracting->GetController<APlayerController>())
	{
		const UPEProjectSettings* const ProjectSettings = GetDefault<UPEProjectSettings>();
		const TSubclassOf<UUserWidget> TradeUIClass = ProjectSettings->TradeInventoryWidget.IsNull() ? nullptr : ProjectSettings->TradeInventoryWidget.LoadSynchronous();

		if (UUserWidget* const TradeWidget = CreateWidget(TargetController, TradeUIClass))
		{
			if (const FObjectProperty* const PackageRef = FindFProperty<FObjectProperty>(TradeUIClass, TEXT("PackageRef")))
			{
				PackageRef->SetPropertyValue_InContainer(TradeWidget, this);
			}

			TradeWidget->AddToPlayerScreen();
		}
	}
}
