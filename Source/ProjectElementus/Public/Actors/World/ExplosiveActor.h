#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExplosiveActor.generated.h"

class UNiagaraSystem;
class UGameplayEffect;
/**
 *
 */
UCLASS(Abstract, Blueprintable, Category = "Custom Classes | Actors")
class PROJECTELEMENTUS_API AExplosiveActor : public AActor
{
	GENERATED_BODY()

public:
	AExplosiveActor(const FObjectInitializer& ObjectInitializer);

	FORCEINLINE virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("Explosive Actor", GetFName());
	}

	UFUNCTION(BlueprintCallable, Category = "Custom Functions | Behaviors")
	void PerformExplosion();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom Properties | Defaults")
	float ExplosionRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom Properties | Defaults")
	float ExplosionMagnitude;

	/* Effects that will be apply to affected characters */
	UPROPERTY(EditDefaultsOnly, Category = "Custom Properties | Defaults")
	TArray<TSubclassOf<UGameplayEffect>> ExplosionEffects;

	/* Visual Effect */
	UPROPERTY(EditDefaultsOnly, Category = "Custom Properties | Defaults")
	TArray<UNiagaraSystem*> ExplosionVFXs;
};
