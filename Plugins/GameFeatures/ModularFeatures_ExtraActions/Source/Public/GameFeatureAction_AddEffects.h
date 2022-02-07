#pragma once

#include "CoreMinimal.h"
#include "GameplayTags.h"
#include "GameplayEffectTypes.h"
#include "GameFeatureAction_WorldActionBase.h"
#include "GameFeatureAction_AddEffects.generated.h"

class UGameplayEffect;
struct FActiveGameplayEffectHandle;
struct FGameplayTag;
struct FComponentRequestHandle;
/**
 * 
 */
USTRUCT(BlueprintType, Category = "GFA Extra Actions | Modular Structs")
struct FEffectStackedData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftClassPtr<UGameplayEffect> EffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 EffectLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FGameplayTag, float> SetByCallerParams;
};

/**
 * 
 */
UCLASS(MinimalAPI, meta = (DisplayName = "Add Effects"))
class UGameFeatureAction_AddEffects final : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Settings")
	TSoftClassPtr<AActor> TargetActorClass;

	UPROPERTY(EditAnywhere, Category="Settings", meta=(TitleProperty="Effects Mapping", ShowOnlyInnerProperties))
	TArray<FEffectStackedData> Effects;

protected:
	virtual void OnGameFeatureActivating() override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
	virtual void AddToWorld(const FWorldContext& WorldContext) override;

private:
	void HandleActorExtension(AActor* Owner, FName EventName);
	void ResetExtension();

	void ApplyEffects(AActor* TargetActor, const FEffectStackedData& Effect);
	void RemoveEffects(AActor* TargetActor);

	TMap<TWeakObjectPtr<AActor>, TArray<FActiveGameplayEffectHandle>> ActiveExtensions;
	TArray<TSharedPtr<FComponentRequestHandle>> ActiveRequests;
};
