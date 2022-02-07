#pragma once

#include "CoreMinimal.h"
#include "GameFeatureAction_WorldActionBase.h"
#include "GameFeatureAction_SpawnActors.generated.h"

struct FComponentRequestHandle;
/**
 * 
 */
USTRUCT(BlueprintType, Category = "GFA Extra Actions | Modular Structs")
struct FActorSpawnSettings
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (OnlyPlaceable))
	TSoftClassPtr<AActor> ActorClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FTransform SpawnTransform;
};

/**
 * 
 */
UCLASS(MinimalAPI, meta = (DisplayName = "Spawn Actors"))
class UGameFeatureAction_SpawnActors final : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Settings")
	TSoftObjectPtr<UWorld> TargetLevel;

	UPROPERTY(EditAnywhere, Category="Settings")
	TArray<FActorSpawnSettings> SpawnSettings;

protected:
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
	virtual void AddToWorld(const FWorldContext& WorldContext) override;

	UFUNCTION(Server, Reliable)
	void SpawnActors(UWorld* WorldReference);
	void SpawnActors_Implementation(UWorld* WorldReference);

	UFUNCTION(Server, Reliable)
	void DestroyActors();
	void DestroyActors_Implementation();

private:
	void ResetExtension();

	TArray<TWeakObjectPtr<AActor>> SpawnedActors;
};
