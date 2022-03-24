// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "GAS/Tasks/SpawnProjectile_Task.h"
#include "Actors/World/ProjectileActor.h"

USpawnProjectile_Task::USpawnProjectile_Task(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = false;
}

USpawnProjectile_Task* USpawnProjectile_Task::SpawnProjectile(UGameplayAbility* OwningAbility,
	const TSubclassOf<AProjectileActor> ClassToSpawn,
	const FTransform SpawnTransform,
	const FVector DirectionToFire,
	const TArray<FGameplayEffectSpecHandle> EffectSpecsArray)
{
	USpawnProjectile_Task* MyObj = NewAbilityTask<USpawnProjectile_Task>(OwningAbility);

	MyObj->ProjectileClass = ClassToSpawn;
	MyObj->ProjectileTransform = SpawnTransform;
	MyObj->ProjectileFireDirection = DirectionToFire;
	MyObj->ProjectileEffectSpecs = EffectSpecsArray;

	return MyObj;
}

void USpawnProjectile_Task::Activate()
{
	Super::Activate();

	if (ensureMsgf(IsValid(Ability), TEXT("%s have a invalid Ability"), *GetName()))
	{
#if __cplusplus > 201402L // Check if C++ > C++14
		if constexpr (&ProjectileClass != nullptr)
#else
		if (&ProjectileClass != nullptr)
#endif
		{
			AProjectileActor* SpawnedProjectile =
				GetWorld()->SpawnActorDeferred<AProjectileActor>(ProjectileClass, ProjectileTransform);

			SpawnedProjectile->DamageEffectSpecHandles = ProjectileEffectSpecs;

			SpawnedProjectile->FinishSpawning(ProjectileTransform);

			if (IsValid(SpawnedProjectile))
			{
				SpawnedProjectile->FireInDirection(ProjectileFireDirection);
			}

			if (ShouldBroadcastAbilityTaskDelegates())
			{
				OnProjectileSpawn.Broadcast(SpawnedProjectile);
			}
		}
	}

	UE_LOG(LogGameplayTasks, Warning, TEXT("Task %s ended"), *GetName());
	EndTask();
}