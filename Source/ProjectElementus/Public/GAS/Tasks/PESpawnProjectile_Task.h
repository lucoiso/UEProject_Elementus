// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "GAS/System/PEEffectData.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "PESpawnProjectile_Task.generated.h"

class APEProjectileActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpawnProjectileDelegate, APEProjectileActor*, SpawnedProjectile);

/**
 *
 */
UCLASS(NotBlueprintable, NotPlaceable, Category = "Project Elementus | Classes")
class PROJECTELEMENTUS_API UPESpawnProjectile_Task final : public UAbilityTask
{
	GENERATED_BODY()

public:
	explicit UPESpawnProjectile_Task(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/* Create a reference to manage this ability task */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UPESpawnProjectile_Task* SpawnProjectile(UGameplayAbility* OwningAbility, const FName TaskInstanceName, TSubclassOf<APEProjectileActor> ClassToSpawn, const FTransform SpawnTransform, const FVector DirectionToFire, const TArray<FGameplayEffectGroupedData> EffectDataArray);

	virtual void Activate() override;

	UPROPERTY(BlueprintAssignable, Category = "Project Elementus | Delegates")
	FSpawnProjectileDelegate OnProjectileSpawn;

	UPROPERTY(BlueprintAssignable, Category = "Project Elementus | Delegates")
	FSpawnProjectileDelegate OnSpawnFailed;

protected:
	FTransform ProjectileTransform;
	FVector ProjectileFireDirection;
	TSubclassOf<APEProjectileActor> ProjectileClass;
	TArray<FGameplayEffectGroupedData> ProjectileEffectArr;
};
