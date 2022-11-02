// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEModularFeatures_ExtraActions

#include "GameFeatureAction_WorldActionBase.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

DEFINE_LOG_CATEGORY(LogGameplayFeaturesExtraActions);

void UGameFeatureAction_WorldActionBase::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	GameInstanceStartHandle = FWorldDelegates::OnStartGameInstance.AddUObject(this, &UGameFeatureAction_WorldActionBase::HandleGameInstanceStart, FGameFeatureStateChangeContext(Context));

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
	FWorldDelegates::OnStartGameInstance.Remove(GameInstanceStartHandle);
}

bool UGameFeatureAction_WorldActionBase::ActorHasAllRequiredTags(const AActor* Actor, const TArray<FName>& RequiredTags)
{
	for (const FName& Tag : RequiredTags)
	{
		if (!Actor->ActorHasTag(Tag))
		{
			return false;
		}
	}

	return true;
}

UGameFrameworkComponentManager* UGameFeatureAction_WorldActionBase::GetGameFrameworkComponentManager(const FWorldContext& WorldContext) const
{
	if (!IsValid(WorldContext.World()) || !WorldContext.World()->IsGameWorld())
	{
		return nullptr;
	}

	return UGameInstance::GetSubsystem<UGameFrameworkComponentManager>(WorldContext.OwningGameInstance);
}

void UGameFeatureAction_WorldActionBase::HandleGameInstanceStart(UGameInstance* GameInstance, const FGameFeatureStateChangeContext ChangeContext)
{
	if (ChangeContext.ShouldApplyToWorldContext(*GameInstance->GetWorldContext()))
	{
		AddToWorld(*GameInstance->GetWorldContext());
	}
}
