// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "GAS/System/PEAbilityData.h"
#include "PESpawnProjectile_Task.generated.h"

class APEProjectileActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpawnProjectileDelegate, APEProjectileActor*, SpawnedProjectile);

/**
 *
 */
UCLASS(NotBlueprintable, NotPlaceable, Category = "Custom GAS | Tasks")
class PROJECTELEMENTUS_API UPESpawnProjectile_Task final : public UAbilityTask
{
	GENERATED_BODY()

public:
	explicit UPESpawnProjectile_Task(const FObjectInitializer& ObjectInitializer);

	/* Create a reference to manage this ability task */
	UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility",
		BlueprintInternalUseOnly = "true"), Category = "Custom GAS | Tasks")
	static UPESpawnProjectile_Task* SpawnProjectile(UGameplayAbility* OwningAbility,
	                                                TSubclassOf<APEProjectileActor> ClassToSpawn,
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
	TSubclassOf<APEProjectileActor> ProjectileClass;
	TArray<FGameplayEffectGroupedData> ProjectileEffectArr;
};
