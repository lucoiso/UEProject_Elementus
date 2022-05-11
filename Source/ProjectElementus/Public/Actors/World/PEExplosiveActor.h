// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GAS/System/PEAbilityData.h"
#include "PEExplosiveActor.generated.h"

class UNiagaraSystem;
class UGameplayEffect;
class UAbilitySystemComponent;
/**
 *
 */
UCLASS(Abstract, Blueprintable, Category = "Project Elementus | Classes")
class PROJECTELEMENTUS_API APEExplosiveActor final : public AActor
{
	GENERATED_BODY()

public:
	explicit APEExplosiveActor(const FObjectInitializer& ObjectInitializer);

	FORCEINLINE virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("Explosive Actor", GetFName());
	}

	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	void PerformExplosion();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Project Elementus | Properties")
	float ExplosionRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Project Elementus | Properties")
	float ExplosionMagnitude;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties")
	bool bDestroyAfterExplosion;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Project Elementus | Properties")
	bool bDebug;

	/* Gameplay Effects and SetByCaller parameters that will be applied to target */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties")
	TArray<FGameplayEffectGroupedData> ExplosionEffects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties")
	TArray<UNiagaraSystem*> ExplosionVFXs;

private:
	void ApplyExplosibleEffect(UAbilitySystemComponent* TargetABSC);
};
