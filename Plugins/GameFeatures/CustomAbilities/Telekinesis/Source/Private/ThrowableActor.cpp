// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "ThrowableActor.h"
#include "Components/StaticMeshComponent.h"

AThrowableActor::AThrowableActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SetRootComponent(GetStaticMeshComponent());
	GetStaticMeshComponent()->SetSimulatePhysics(true);
	GetStaticMeshComponent()->SetGenerateOverlapEvents(true);
	GetStaticMeshComponent()->SetCollisionObjectType(ECC_PhysicsBody);
	GetStaticMeshComponent()->SetCollisionProfileName(TEXT("PhysicsBody"));
	GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	SetMobility(EComponentMobility::Movable);

	bReplicates = true;
	GetStaticMeshComponent()->SetIsReplicated(true);
}