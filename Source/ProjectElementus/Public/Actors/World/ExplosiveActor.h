// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Management/Data/GASAbilityData.h"
#include "ExplosiveActor.generated.h"

class UNiagaraSystem;
class UGameplayEffect;
class UAbilitySystemComponent;
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Properties | Defaults")
		float ExplosionRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Properties | Defaults")
		float ExplosionMagnitude;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom Properties | Defaults")
		bool bDestroyAfterExplosion;

	/* Effects that will be apply to affected characters */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom Properties | Defaults")
		TArray<FGameplayEffectGroupedData> ExplosionEffects;

	/* Visual Effect */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom Properties | Defaults")
		TArray<UNiagaraSystem*> ExplosionVFXs;

private:
	void ApplyExplosibleEffect(UAbilitySystemComponent* TargetABSC);
};
