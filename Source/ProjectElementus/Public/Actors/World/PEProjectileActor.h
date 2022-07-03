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
UCLASS(Abstract, Blueprintable, Category = "Project Elementus | Classes")
class PROJECTELEMENTUS_API APEProjectileActor final : public AActor
{
	GENERATED_BODY()

public:
	explicit APEProjectileActor(const FObjectInitializer& ObjectInitializer);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties")
	TObjectPtr<USphereComponent> CollisionComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties")
	float ImpulseMultiplier = 1.f;

	virtual void BeginPlay() override;

public:
	/* Gameplay Effects and SetByCaller parameters that will be applied to target */
	UPROPERTY(BlueprintReadOnly, Meta = (ExposeOnSpawn = true), Category = "Project Elementus | Properties")
	TArray<FGameplayEffectGroupedData> ProjectileEffects;

	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	void FireInDirection(const FVector Direction) const;

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Project Elementus | Functions")
	void OnProjectileHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                     FVector NormalImpulse, const FHitResult& Hit);

private:
	void ApplyProjectileEffect(UAbilitySystemComponent* TargetABSC);
};
