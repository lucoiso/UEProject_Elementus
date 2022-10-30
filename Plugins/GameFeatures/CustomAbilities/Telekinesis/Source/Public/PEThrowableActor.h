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
UCLASS(MinimalAPI, Abstract, Blueprintable, Category = "Project Elementus | Classes")
class APEThrowableActor final : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	explicit APEThrowableActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void ThrowSetup(AActor* Caller);

protected:
	/* Effects that will be apply to affected characters on Hit */
	UPROPERTY(EditDefaultsOnly, Category = "Project Elementus | Properties")
	TArray<FGameplayEffectGroupedData> HitEffects;

	UFUNCTION()
	void OnThrowableHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void ApplyThrowableEffect(UAbilitySystemComponent* TargetABSC);

private:
	TWeakObjectPtr<AActor> CallerActor;
};
