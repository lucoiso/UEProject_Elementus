// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "ThrowableActor.generated.h"

class UGameplayEffect;
/**
 *
 */
UCLASS(MinimalAPI, Abstract, Blueprintable, Category = "Custom Classes | Actors")
class AThrowableActor : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	AThrowableActor(const FObjectInitializer& ObjectInitializer);

	FORCEINLINE virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("Throwable Actor", GetFName());
	}

	void ThrowSetup(AActor* Caller);

protected:
	/* Effects that will be apply to affected characters on Hit */
	UPROPERTY(EditDefaultsOnly, Category = "Custom Properties | Defaults")
		TArray<TSubclassOf<UGameplayEffect>> HitEffects;

private:
	UFUNCTION()
		void OnThrowableHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(Server, Reliable, WithValidation, Category = "Custom Functions | Behaviors")
		void ApplyThrowableEffect(UAbilitySystemComponent* TargetComp);
	virtual void ApplyThrowableEffect_Implementation(UAbilitySystemComponent* TargetComp);
	bool ApplyThrowableEffect_Validate(UAbilitySystemComponent* TargetComp);

	TWeakObjectPtr<AActor> CallerActor;
};
