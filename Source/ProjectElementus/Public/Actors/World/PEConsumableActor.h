// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GAS/System/PEAbilityData.h"
#include "GameFramework/Actor.h"
#include "PEConsumableActor.generated.h"

/**
 *
 */
UCLASS(Abstract, Blueprintable, Category = "Custom Classes | Actors")
class PROJECTELEMENTUS_API APEConsumableActor final : public AActor
{
	GENERATED_BODY()

public:
	explicit APEConsumableActor(const FObjectInitializer& ObjectInitializer);

	FORCEINLINE virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("Consumable Actor", GetFName());
	}

	UFUNCTION(BlueprintCallable, Category = "Custom Functions | Behaviors")
	void PerformConsumption(class UAbilitySystemComponent* TargetABSC, const bool bDestroyAfterConsumption);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom Properties | Defaults")
	UStaticMeshComponent* ObjectMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom Properties | Defaults")
	class UNiagaraComponent* ObjectVFX;

	/* Gameplay Effects and SetByCaller parameters that will be applied to target */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom Properties | Defaults")
	TArray<FGameplayEffectGroupedData> ConsumableEffects;

	/* Tags that target require to consume this */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom Properties | Defaults")
	FGameplayTagContainer RequirementsTags;
};
