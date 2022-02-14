// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEModularFeatures_ExtraActions

#include "GameFeatureAction_SpawnActors.h"
#include "Components/GameFrameworkComponentManager.h"

void UGameFeatureAction_SpawnActors::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);

	ResetExtension();
}

void UGameFeatureAction_SpawnActors::ResetExtension()
{
	DestroyActors();
}

void UGameFeatureAction_SpawnActors::AddToWorld(const FWorldContext& WorldContext)
{
	if (!TargetLevel.IsNull())
	{
		UWorld* World = WorldContext.World();

		if (IsValid(World) && World->IsGameWorld() && World == TargetLevel.Get())
		{
			SpawnActors(World);
		}
	}
}

void UGameFeatureAction_SpawnActors::SpawnActors_Implementation(UWorld* WorldReference)
{
	for (const FActorSpawnSettings& Settings : SpawnSettings)
	{
		UE_LOG(LogGameplayExtraFeatures, Warning,
		       TEXT("Spawning actor %s on world %s"), *Settings.ActorClass.GetAssetName(), *TargetLevel.GetAssetName());

		AActor* SpawnedActor = WorldReference->SpawnActor<AActor>(Settings.ActorClass.LoadSynchronous(),
		                                                          Settings.SpawnTransform);
		SpawnedActors.Add(SpawnedActor);
	}
}

void UGameFeatureAction_SpawnActors::DestroyActors_Implementation()
{
	for (const TWeakObjectPtr<AActor>& ActorPtr : SpawnedActors)
	{
		if (ActorPtr.IsValid())
		{
			ActorPtr->Destroy();
		}
	}

	SpawnedActors.Empty();
}
