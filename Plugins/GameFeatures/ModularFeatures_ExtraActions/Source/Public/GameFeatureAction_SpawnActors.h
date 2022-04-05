// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEModularFeatures_ExtraActions

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

public:
	/* Actor class to be spawned */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (OnlyPlaceable))
		TSoftClassPtr<AActor> ActorClass;

	/* Transform settings to be added to spawned actor */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FTransform SpawnTransform;
};

/**
 *
 */
UCLASS(MinimalAPI, meta = (DisplayName = "GFA Extra Actions: Spawn Actors"))
class UGameFeatureAction_SpawnActors final : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()

public:
	/* Target level to which actor will be spawned */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
		TSoftObjectPtr<UWorld> TargetLevel;

	/* Stacked spawn settings */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
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
