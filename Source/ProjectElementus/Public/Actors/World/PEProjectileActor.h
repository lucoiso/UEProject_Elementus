// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GAS/System/PEAbilityData.h"
#include "PEProjectileActor.generated.h"

class UAbilitySystemComponent;
class USphereComponent;
class UProjectileMovementComponent;
/**
 *
 */
UCLASS(Abstract, Blueprintable, Category = "Custom Classes | Actors")
class PROJECTELEMENTUS_API APEProjectileActor : public AActor
{
	GENERATED_BODY()

public:
	APEProjectileActor(const FObjectInitializer& ObjectInitializer);

	FORCEINLINE virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("Projectile Actor", GetFName());
	}

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom Properties | Defaults")
		USphereComponent* CollisionComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom Properties | Defaults")
		UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom Properties | Defaults")
		float ImpulseMultiplier = 1.f;

	virtual void BeginPlay() override;

public:
	/* Gameplay Effects and SetByCaller parameters that will be applied to target */
	UPROPERTY(BlueprintReadOnly, Meta = (ExposeOnSpawn = true), Category = "Custom Properties | Defaults")
		TArray<FGameplayEffectGroupedData> ProjectileEffects;

	UFUNCTION(BlueprintCallable, Category = "Custom Functions | Behaviors")
		virtual void FireInDirection(const FVector Direction);

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Custom Functions | Behaviors")
		void OnProjectileHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			FVector NormalImpulse, const FHitResult& Hit);

private:
	void ApplyProjectileEffect(UAbilitySystemComponent* TargetABSC);
};
