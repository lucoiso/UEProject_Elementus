// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "GAS/Tasks/PESpawnProjectile_Task.h"
#include "Actors/World/PEProjectileActor.h"

UPESpawnProjectile_Task::UPESpawnProjectile_Task(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = false;
}

UPESpawnProjectile_Task* UPESpawnProjectile_Task::SpawnProjectile(UGameplayAbility* OwningAbility,
	const TSubclassOf<APEProjectileActor> ClassToSpawn,
	const FTransform SpawnTransform,
	const FVector DirectionToFire,
	TArray<FGameplayEffectGroupedData> EffectDataArray)
{
	UPESpawnProjectile_Task* MyObj = NewAbilityTask<UPESpawnProjectile_Task>(OwningAbility);

	MyObj->ProjectileClass = ClassToSpawn;
	MyObj->ProjectileTransform = SpawnTransform;
	MyObj->ProjectileFireDirection = DirectionToFire;
	MyObj->ProjectileEffectArr = EffectDataArray;

	return MyObj;
}

void UPESpawnProjectile_Task::Activate()
{
	Super::Activate();

	if (ensureMsgf(IsValid(Ability), TEXT("%s have a invalid Ability"), *GetName()))
	{
		if (Ability->GetActorInfo().IsNetAuthority())
		{
			if (ProjectileClass != nullptr)
			{
				APEProjectileActor* SpawnedProjectile =
					GetWorld()->SpawnActorDeferred<APEProjectileActor>(ProjectileClass, ProjectileTransform,
						Ability->GetAvatarActorFromActorInfo(),
						Ability->GetActorInfo().PlayerController->
						GetPawn(),
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