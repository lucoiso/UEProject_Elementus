// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEModularFeatures_ExtraActions

#pragma once

#include "CoreMinimal.h"
#include "GameFeatureAction.h"
#include "GameFeatureAction_WorldActionBase.generated.h"

class UGameInstance;
struct FWorldContext;

DECLARE_LOG_CATEGORY_EXTERN(LogGameplayExtraFeatures, Display, All);

UENUM(BlueprintType, Category = "GFA Extra Actions | Enums")
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

public:
	virtual void OnGameFeatureActivating() override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;

protected:
	virtual void AddToWorld(const FWorldContext& WorldContext)
	{
	};

private:
	void HandleGameInstanceStart(UGameInstance* GameInstance);
	FDelegateHandle GameInstanceStartHandle;
};
