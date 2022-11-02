// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEModularFeatures_ExtraActions

#pragma once

#include "CoreMinimal.h"
#include "GameFeatureAction.h"
#include "GameFeaturesSubsystem.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameFeatureAction_WorldActionBase.generated.h"

class UGameInstance;
struct FWorldContext;

DECLARE_LOG_CATEGORY_EXTERN(LogGameplayFeaturesExtraActions, Display, All);

using FComponentRequestHandlePtr = TSharedPtr<FComponentRequestHandle>;

UENUM(BlueprintType, Category = "MF Extra Actions | Enums")
enum class EControllerOwner :uint8
{
	Pawn,
	Controller
};

/**
 *
 */
UCLASS(MinimalAPI, Abstract)
class UGameFeatureAction_WorldActionBase : public UGameFeatureAction
{
	GENERATED_BODY()

protected:
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;

	virtual void AddToWorld(const FWorldContext& WorldContext)
	{
	}

	static bool ActorHasAllRequiredTags(const AActor* Actor, const TArray<FName>& RequiredTags);

	UGameFrameworkComponentManager* GetGameFrameworkComponentManager(const FWorldContext& WorldContext) const;
	TArray<FComponentRequestHandlePtr> ActiveRequests;

private:
	void HandleGameInstanceStart(UGameInstance* GameInstance, FGameFeatureStateChangeContext ChangeContext);
	FDelegateHandle GameInstanceStartHandle;
};
