// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "ProjectileActor.generated.h"

class UAbilitySystemComponent;
class USphereComponent;
class UProjectileMovementComponent;
/**
 *
 */
UCLASS(Abstract, Blueprintable, Category = "Custom Classes | Actors")
class PROJECTELEMENTUS_API AProjectileActor : public AActor
{
	GENERATED_BODY()

public:
	AProjectileActor(const FObjectInitializer& ObjectInitializer);

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
	UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = "Custom Properties | Defaults")
	TArray<FGameplayEffectSpecHandle> DamageEffectSpecHandles;

	UFUNCTION(BlueprintCallable, Category = "Custom Functions | Behaviors")
	virtual void FireInDirection(const FVector Direction);

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Custom Functions | Behaviors")
	void OnProjectileHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                     FVector NormalImpulse, const FHitResult& Hit);

private:
	UFUNCTION(Server, Reliable, WithValidation, Category = "Custom Functions | Behaviors")
	void ApplyProjectileEffect(UAbilitySystemComponent* TargetComp);
	void ApplyProjectileEffect_Implementation(UAbilitySystemComponent* TargetComp);
	bool ApplyProjectileEffect_Validate(UAbilitySystemComponent* TargetComp);
};
