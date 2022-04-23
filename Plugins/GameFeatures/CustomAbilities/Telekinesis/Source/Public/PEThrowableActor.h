// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "GAS/System/PEAbilityData.h"
#include "PEThrowableActor.generated.h"

class UGameplayEffect;
class UAbilitySystemComponent;
/**
 *
 */
UCLASS(MinimalAPI, Abstract, Blueprintable, Category = "Custom Classes | Actors")
class APEThrowableActor final : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	explicit APEThrowableActor(const FObjectInitializer& ObjectInitializer);

	FORCEINLINE virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("Throwable Actor", GetFName());
	}

	void ThrowSetup(AActor* Caller);

protected:
	/* Effects that will be apply to affected characters on Hit */
	UPROPERTY(EditDefaultsOnly, Category = "Custom Properties | Defaults")
	TArray<FGameplayEffectGroupedData> HitEffects;

private:
	UFUNCTION()
	void OnThrowableHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                    FVector NormalImpulse, const FHitResult& Hit);

	void ApplyThrowableEffect(UAbilitySystemComponent* TargetABSC);
	TWeakObjectPtr<AActor> CallerActor;
};
