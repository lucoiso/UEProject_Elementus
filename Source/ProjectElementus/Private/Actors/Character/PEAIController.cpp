#include "Actors/Character/PEAIController.h"
#include "Components/GameFrameworkComponentManager.h"

APEAIController::APEAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void APEAIController::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void APEAIController::BeginPlay()
{
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(
		this, UGameFrameworkComponentManager::NAME_GameActorReady);

	Super::BeginPlay();
}

void APEAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);

	Super::EndPlay(EndPlayReason);
}
