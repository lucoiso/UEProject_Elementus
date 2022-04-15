// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Management/Data/GASAbilityData.h"
#include "SpawnProjectile_Task.generated.h"

class AProjectileActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpawnProjectileDelegate, AProjectileActor*, SpawnedProjectile);

/**
 *
 */
UCLASS(NotBlueprintable, NotPlaceable, Category = "Custom GAS | Tasks")
class PROJECTELEMENTUS_API USpawnProjectile_Task : public UAbilityTask
{
	GENERATED_BODY()

public:
	USpawnProjectile_Task(const FObjectInitializer& ObjectInitializer);

	/* Create a reference to manage this ability task */
	UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility",
		BlueprintInternalUseOnly = "true"), Category = "Custom GAS | Tasks")
		static USpawnProjectile_Task* SpawnProjectile(UGameplayAbility* OwningAbility,
			TSubclassOf<AProjectileActor> ClassToSpawn,
			const FTransform SpawnTransform,
			const FVector DirectionToFire,
			const TArray<FGameplayEffectGroupedData> EffectDataArray);

	virtual void Activate() override;

	UPROPERTY(BlueprintAssignable, Category = "Custom GAS | Delegates")
		FSpawnProjectileDelegate OnProjectileSpawn;

	UPROPERTY(BlueprintAssignable, Category = "Custom GAS | Delegates")
		FSpawnProjectileDelegate OnSpawnFailed;

protected:
	FTransform ProjectileTransform;
	FVector ProjectileFireDirection;
	TSubclassOf<AProjectileActor> ProjectileClass;
	TArray<FGameplayEffectGroupedData> ProjectileEffectArr;
};
