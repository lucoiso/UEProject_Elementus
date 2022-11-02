// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEModularFeatures_ExtraActions

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h"
#include "GameFeatureAction_WorldActionBase.h"
#include "GameFeatureAction_AddEffects.generated.h"

class UGameplayEffect;
struct FActiveGameplayEffectHandle;
struct FComponentRequestHandle;

/**
 *
 */
USTRUCT(BlueprintType, Category = "MF Extra Actions | Modular Structs")
struct FEffectStackedData
{
	GENERATED_BODY()

	/* Gameplay Effect Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	TSoftClassPtr<UGameplayEffect> EffectClass;

	/* Gameplay Effect level */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	int32 EffectLevel = 1;

	/* Set By Caller parameters */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (TitleProperty = "{SetByCaller Tag} -> {SetByCaller Value}"))
	TMap<FGameplayTag, float> SetByCallerParams;
};

/**
 *
 */
UCLASS(MinimalAPI, meta = (DisplayName = "MF Extra Actions: Add Effects"))
class UGameFeatureAction_AddEffects final : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()

public:
	/* Target pawn to which gameplay effects will be given */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (AllowedClasses = "/Script/Engine.Pawn", OnlyPlaceable = "true"))
	TSoftClassPtr<APawn> TargetPawnClass;

	/* Tags required on the target to apply this action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	TArray<FName> RequireTags;

	/* Gameplay Effects stacked informations */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (DisplayName = "Effects Mapping", ShowOnlyInnerProperties))
	TArray<FEffectStackedData> Effects;

protected:
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
	virtual void AddToWorld(const FWorldContext& WorldContext) override;

private:
	void HandleActorExtension(AActor* Owner, FName EventName);
	void ResetExtension();

	void AddEffects(AActor* TargetActor, const FEffectStackedData& Effect);
	void RemoveEffects(AActor* TargetActor);

	TMap<TWeakObjectPtr<AActor>, TArray<FActiveGameplayEffectHandle>> ActiveExtensions;
};
