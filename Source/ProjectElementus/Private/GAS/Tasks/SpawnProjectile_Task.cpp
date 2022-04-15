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
	TArray<FGameplayEffectGroupedData> EffectDataArray)
{
	USpawnProjectile_Task* MyObj = NewAbilityTask<USpawnProjectile_Task>(OwningAbility);

	MyObj->ProjectileClass = ClassToSpawn;
	MyObj->ProjectileTransform = SpawnTransform;
	MyObj->ProjectileFireDirection = DirectionToFire;
	MyObj->ProjectileEffectArr = EffectDataArray;

	return MyObj;
}

void USpawnProjectile_Task::Activate()
{
	Super::Activate();

	if (ensureMsgf(IsValid(Ability), TEXT("%s have a invalid Ability"), *GetName()))
	{
		if (Ability->GetActorInfo().IsNetAuthority())
		{
			if (ProjectileClass != nullptr)
			{
				AProjectileActor* SpawnedProjectile =
					GetWorld()->SpawnActorDeferred<AProjectileActor>(ProjectileClass, ProjectileTransform,
						Ability->GetAvatarActorFromActorInfo(), Ability->GetActorInfo().PlayerController->GetPawn(),
						ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

				SpawnedProjectile->ProjectileEffects = ProjectileEffectArr;

				SpawnedProjectile->FinishSpawning(ProjectileTransform);

				if (IsValid(SpawnedProjectile))
				{
					SpawnedProjectile->FireInDirection(ProjectileFireDirection);

					if (ShouldBroadcastAbilityTaskDelegates())
					{
						OnProjectileSpawn.Broadcast(SpawnedProjectile);
					}
				}
				else if (ShouldBroadcastAbilityTaskDelegates())
				{
					OnSpawnFailed.Broadcast(nullptr);
				}
			}
			else if (ShouldBroadcastAbilityTaskDelegates())
			{
				OnSpawnFailed.Broadcast(nullptr);
			}
		}
	}

	UE_LOG(LogGameplayTasks, Warning, TEXT("Task %s ended"), *GetName());
	EndTask();
}