// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEModularFeatures_ExtraActions

#include "GameFeatureAction_WorldActionBase.h"
#include "GameFeaturesSubsystemSettings.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

DEFINE_LOG_CATEGORY(LogGameplayExtraFeatures);

void UGameFeatureAction_WorldActionBase::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	ContextHandles.AddUnique(Context);

	GameInstanceStartHandle = FWorldDelegates::OnStartGameInstance.AddUObject(
		this, &UGameFeatureAction_WorldActionBase::HandleGameInstanceStart, FGameFeatureStateChangeContext(Context));

	for (const FWorldContext& WorldContext : GEngine->GetWorldContexts())
	{
		if (Context.ShouldApplyToWorldContext(WorldContext))
		{
			AddToWorld(WorldContext);
		}
	}
}

void UGameFeatureAction_WorldActionBase::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	ContextHandles.AddUnique(Context);

	FWorldDelegates::OnStartGameInstance.Remove(GameInstanceStartHandle);

	ContextHandles.Empty();
}

void UGameFeatureAction_WorldActionBase::HandleGameInstanceStart(UGameInstance* GameInstance, FGameFeatureStateChangeContext ChangeContext)
{
	if (const FWorldContext* WorldContext = GameInstance->GetWorldContext())
	{
		if (ChangeContext.ShouldApplyToWorldContext(*WorldContext))
		{
			AddToWorld(*WorldContext);
		}
	}
}