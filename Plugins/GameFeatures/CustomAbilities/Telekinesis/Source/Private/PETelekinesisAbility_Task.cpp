// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "PETelekinesisAbility_Task.h"
#include "PEThrowableActor.h"
#include <GAS/Targeting/PELineTargeting.h>
#include <Actors/Character/PECharacter.h>
#include <PhysicsEngine/PhysicsHandleComponent.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(PETelekinesisAbility_Task)

UPETelekinesisAbility_Task::UPETelekinesisAbility_Task(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bTickingTask = false;
	bIsFinished = false;
}

UPETelekinesisAbility_Task* UPETelekinesisAbility_Task::PETelekinesisAbilityMovement(UGameplayAbility* OwningAbility, const FName TaskInstanceName, const float ThrowIntensity, AActor* Target)
{
	UPETelekinesisAbility_Task* const MyObj = NewAbilityTask<UPETelekinesisAbility_Task>(OwningAbility, TaskInstanceName);
	MyObj->TelekinesisTarget = Target;
	MyObj->Intensity = ThrowIntensity;

	return MyObj;
}

void UPETelekinesisAbility_Task::Activate()
{
	Super::Activate();

	check(Ability);

	TelekinesisOwner = Cast<APECharacter>(GetAvatarActor());

	if (ensureAlwaysMsgf(TelekinesisOwner.IsValid(), TEXT("%s - Task %s failed to activate because have a invalid owner"), *FString(__func__), *GetName()))
	{
		PhysicsHandle = NewObject<UPhysicsHandleComponent>(TelekinesisOwner.Get(), UPhysicsHandleComponent::StaticClass(), TEXT("TelekinesisPhysicsHandle"));

		if (PhysicsHandle.IsValid())
		{
			PhysicsHandle->RegisterComponent();
			PhysicsHandle->GrabComponentAtLocation(Cast<UPrimitiveComponent>(TelekinesisTarget->GetRootComponent()), NAME_None, TelekinesisTarget->GetActorLocation());

			if (IsValid(PhysicsHandle->GetGrabbedComponent()))
			{
				PhysicsHandle->GetGrabbedComponent()->WakeAllRigidBodies();

				if (ShouldBroadcastAbilityTaskDelegates())
				{
					OnGrabbing.ExecuteIfBound(true);
				}

				PhysicsHandle->SetTargetLocation(TelekinesisOwner->GetMesh()->GetSocketLocation("Telekinesis_AbilitySocket"));

				bTickingTask = true;
				return;
			}
		}
	}

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnGrabbing.ExecuteIfBound(false);
	}

	bIsFinished = true;
	EndTask();
}

void UPETelekinesisAbility_Task::TickTask(const float DeltaTime)
{
	if (bIsFinished)
	{
		EndTask();
		return;
	}

	Super::TickTask(DeltaTime);

	if (IsValid(PhysicsHandle->GetGrabbedComponent()))
	{
		PhysicsHandle->SetTargetLocation(TelekinesisOwner->GetMesh()->GetSocketLocation("Telekinesis_AbilitySocket"));
	}

	else
	{
		bIsFinished = true;
		EndTask();
	}
}

void UPETelekinesisAbility_Task::OnDestroy(const bool AbilityIsEnding)
{
	UE_LOG(LogGameplayTasks, Display, TEXT("%s - Task %s ended"), *FString(__func__), *GetName());

	bIsFinished = true;

	if (PhysicsHandle.IsValid())
	{
		UPrimitiveComponent* const GrabbedComponent = PhysicsHandle->GetGrabbedComponent();
		PhysicsHandle->ReleaseComponent();

		if (IsValid(GrabbedComponent))
		{
			GrabbedComponent->WakeAllRigidBodies();
		}
	}

	PhysicsHandle.Reset();

	TelekinesisOwner.Reset();
	TelekinesisTarget.Reset();

	Super::OnDestroy(AbilityIsEnding);
}

void UPETelekinesisAbility_Task::ThrowObject()
{
	bIsFinished = true;

	if (UPrimitiveComponent* const GrabbedPrimitive_Temp = PhysicsHandle->GetGrabbedComponent())
	{
		PhysicsHandle->ReleaseComponent();

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(Ability->GetAvatarActorFromActorInfo());
		QueryParams.AddIgnoredActor(GrabbedPrimitive_Temp->GetAttachmentRootActor());

		const FVector StartLocation = TelekinesisOwner->GetCameraComponentLocation();
		const FVector EndLocation = StartLocation + TelekinesisOwner->GetCameraForwardVector() * 999999.f;

		FHitResult HitResult;
		FGameplayTargetDataFilterHandle DataFilterHandle;

		APELineTargeting::LineTraceWithFilter(HitResult, GetWorld(), DataFilterHandle, StartLocation, EndLocation, TEXT("Target"), QueryParams);

		const FVector Temp_EndLoc = HitResult.bBlockingHit ? HitResult.ImpactPoint : EndLocation;
		const FVector Direction = (Temp_EndLoc - GrabbedPrimitive_Temp->GetComponentLocation()).GetSafeNormal();
		const FVector Velocity = Direction * Intensity;

		GrabbedPrimitive_Temp->SetAllPhysicsLinearVelocity(Velocity);

		if (APEThrowableActor* const Throwable = Cast<APEThrowableActor>(GrabbedPrimitive_Temp->GetAttachmentRootActor()))
		{
			Throwable->ThrowSetup(Ability->GetAvatarActorFromActorInfo());
		}
	}

	EndTask();
}

AActor* UPETelekinesisAbility_Task::GetTelekinesisTarget() const
{
	return TelekinesisTarget.Get();
}
