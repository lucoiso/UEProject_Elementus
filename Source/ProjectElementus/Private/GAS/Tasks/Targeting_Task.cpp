// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "GAS/Tasks/Targeting_Task.h"
#include "Abilities/GameplayAbilityTargetActor_GroundTrace.h"
#include "Abilities/GameplayAbilityTargetActor_Radius.h"

UTargeting_Task* UTargeting_Task::StartTargetingAndWaitData(UGameplayAbility* OwningAbility, const FName TaskInstanceName,
	const TEnumAsByte<EGameplayTargetingConfirmation::Type> ConfirmationType,
	const TSubclassOf<AGameplayAbilityTargetActor> TargetActorClass,
	const FTargetActorSpawnParams Parameters)
{
	UTargeting_Task* MyObj = NewAbilityTask<UTargeting_Task>(OwningAbility, TaskInstanceName);
	MyObj->TargetActorClass = TargetActorClass;
	MyObj->ConfirmationType = ConfirmationType;
	MyObj->TargetingParams = Parameters;
	MyObj->bTickingTask = true;

	return MyObj;
}

void UTargeting_Task::Activate()
{
	Super::Activate();

	if (TargetActorClass.Get() == AGameplayAbilityTargetActor_GroundTrace::StaticClass())
	{
		AGameplayAbilityTargetActor_GroundTrace* GroundTraceObj =
			GetWorld()->SpawnActorDeferred<AGameplayAbilityTargetActor_GroundTrace>(TargetActorClass, FTransform::Identity);

		GroundTraceObj->CollisionRadius = TargetingParams.Radius;
		GroundTraceObj->CollisionHeight = TargetingParams.Height;
		GroundTraceObj->MaxRange = TargetingParams.Range;
		GroundTraceObj->bTraceAffectsAimPitch = TargetingParams.bTraceAffectsAimPitch;

		TargetActor = GroundTraceObj;
	}
	else if (TargetActorClass.Get() == AGameplayAbilityTargetActor_Radius::StaticClass())
	{
		AGameplayAbilityTargetActor_Radius* RadiusTargetObj =
			GetWorld()->SpawnActorDeferred<AGameplayAbilityTargetActor_Radius>(TargetActorClass, FTransform::Identity);

		RadiusTargetObj->Radius = TargetingParams.Radius;
	}
	else
	{
		TargetActor = GetWorld()->SpawnActorDeferred<AGameplayAbilityTargetActor>(TargetActorClass, FTransform::Identity);
	}

	TargetActor->StartLocation = TargetingParams.StartLocation;
	TargetActor->ReticleClass = TargetingParams.ReticleClass;
	TargetActor->ReticleParams = TargetingParams.ReticleParams;

	FGameplayTargetDataFilterHandle FilterHandle;
	FilterHandle.Filter = MakeShared<FGameplayTargetDataFilter>(TargetingParams.TargetFilter);
	TargetActor->Filter = FilterHandle;

	TargetActor->bDestroyOnConfirmation = TargetingParams.bDestroyOnConfirmation;
	TargetActor->bDebug = TargetingParams.bDebug;

	TargetActor->FinishSpawning(FTransform::Identity);

	if (TargetActor.IsValid())
	{
		TargetActor->MasterPC = Ability->GetCurrentActorInfo()->PlayerController.Get();

		TargetActor->TargetDataReadyDelegate.AddUObject(this, &UTargeting_Task::OnTargetDataReadyCallback);
		TargetActor->CanceledDelegate.AddUObject(this, &UTargeting_Task::OnTargetDataCancelledCallback);

		TargetActor->StartTargeting(Ability);

		if (TargetActor->ShouldProduceTargetData())
		{
			if (ConfirmationType == EGameplayTargetingConfirmation::Instant)
			{
				TargetActor->ConfirmTargeting();
			}
			else
			{
				TargetActor->BindToConfirmCancelInputs();
			}
		}
	}
	else
	{
		EndTask();
	}
}

void UTargeting_Task::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& Data)
{
	ValidData.Broadcast(Data);
}

void UTargeting_Task::OnTargetDataCancelledCallback(const FGameplayAbilityTargetDataHandle& Data)
{
	Cancelled.Broadcast(Data);
}

void UTargeting_Task::OnDestroy(bool AbilityEnded)
{
	TargetActor->TargetDataReadyDelegate.RemoveAll(this);
	TargetActor->CanceledDelegate.RemoveAll(this);

	TargetActor->Destroy();
	TargetActor.Reset();

	Super::OnDestroy(AbilityEnded);
}

void UTargeting_Task::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (TargetActor.IsValid())
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.001f, FColor::Yellow, "ShouldProduceTargetData: " + FString::FromInt(TargetActor->ShouldProduceTargetData()));
		GEngine->AddOnScreenDebugMessage(-1, 0.001f, FColor::Yellow, "ShouldProduceTargetDataOnServer: " + FString::FromInt(TargetActor->ShouldProduceTargetDataOnServer));
		GEngine->AddOnScreenDebugMessage(-1, 0.001f, FColor::Yellow, "IsConfirmTargetingAllowed: " + FString::FromInt(TargetActor->IsConfirmTargetingAllowed()));
		GEngine->AddOnScreenDebugMessage(-1, 0.001f, FColor::Yellow, "GetTargetLocation: " + TargetActor->GetTargetLocation().ToString());
		GEngine->AddOnScreenDebugMessage(-1, 0.001f, FColor::Yellow, "TargetDataReadyDelegate.IsBound(): " + FString::FromInt(TargetActor->TargetDataReadyDelegate.IsBound()));
		GEngine->AddOnScreenDebugMessage(-1, 0.001f, FColor::Yellow, "CanceledDelegate.IsBound(): " + FString::FromInt(TargetActor->CanceledDelegate.IsBound()));
	}
}