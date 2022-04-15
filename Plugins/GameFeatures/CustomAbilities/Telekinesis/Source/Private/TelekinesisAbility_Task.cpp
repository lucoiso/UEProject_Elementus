// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "TelekinesisAbility_Task.h"
#include "ThrowableActor.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Actors/Character/PECharacterBase.h"
#include "Abilities/GameplayAbilityTargetActor_Trace.h"

UTelekinesisAbility_Task::UTelekinesisAbility_Task(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = false;
	bIsFinished = false;
}

UTelekinesisAbility_Task* UTelekinesisAbility_Task::TelekinesisAbilityMovement(
	UGameplayAbility* OwningAbility, const FName TaskInstanceName, const TWeakObjectPtr<AActor> Target)
{
	UTelekinesisAbility_Task* MyObj = NewAbilityTask<UTelekinesisAbility_Task>(OwningAbility, TaskInstanceName);
	MyObj->TelekinesisTarget = Target;
	return MyObj;
}

void UTelekinesisAbility_Task::Activate()
{
	Super::Activate();

	if (ensureMsgf(IsValid(Ability), TEXT("%s have a invalid Ability"), *GetName()))
	{
		if (Ability->GetActorInfo().IsNetAuthority())
		{
			TelekinesisOwner = Cast<APECharacterBase>(Ability->GetAvatarActorFromActorInfo());

			if (ensureMsgf(TelekinesisOwner.IsValid(), TEXT("%s have a invalid Owner"), *GetName()))
			{
				PhysicsHandle = NewObject<UPhysicsHandleComponent>(TelekinesisOwner.Get(), UPhysicsHandleComponent::StaticClass(),
					FName("TelekinesisPhysicsHandle"));

				if (PhysicsHandle.IsValid())
				{
					PhysicsHandle->RegisterComponent();
					PhysicsHandle->GrabComponentAtLocation(Cast<UPrimitiveComponent>(TelekinesisTarget->GetRootComponent()),
						NAME_None, TelekinesisTarget->GetActorLocation());

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
		}
	}

	bIsFinished = true;
	EndTask();
}

void UTelekinesisAbility_Task::TickTask(const float DeltaTime)
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

void UTelekinesisAbility_Task::OnDestroy(const bool AbilityIsEnding)
{
	UE_LOG(LogGameplayTasks, Warning, TEXT("Task %s ended"), *GetName());
	
	bIsFinished = true;

	if (PhysicsHandle.IsValid() && IsValid(PhysicsHandle->GetGrabbedComponent()))
	{
		PhysicsHandle->ReleaseComponent();
	}

	PhysicsHandle.Reset();

	TelekinesisOwner.Reset();
	TelekinesisTarget.Reset();

	Super::OnDestroy(AbilityIsEnding);
}

void UTelekinesisAbility_Task::ThrowObject()
{
	UE_LOG(LogGameplayTasks, Warning, TEXT(" %s called"), *FString(__func__));

	bIsFinished = true;

	UPrimitiveComponent* GrabbedPrimitive_Temp = PhysicsHandle->GetGrabbedComponent();

	if (ensureMsgf(IsValid(GrabbedPrimitive_Temp), TEXT("%s have a invalid Owner"), *GetName()))
	{
		PhysicsHandle->ReleaseComponent();

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(Ability->GetAvatarActorFromActorInfo());
		QueryParams.AddIgnoredActor(GrabbedPrimitive_Temp->GetAttachmentRootActor());

		FVector StartLocation = TelekinesisOwner->GetCameraComponentLocation();
		FVector EndLocation = StartLocation + (TelekinesisOwner->GetCameraForwardVector() * 999999.f);

		FHitResult HitResult;
		FGameplayTargetDataFilterHandle DataFilterHandle;

		AGameplayAbilityTargetActor_Trace::LineTraceWithFilter(HitResult, GetWorld(), DataFilterHandle, StartLocation, EndLocation, "None", QueryParams);

		const FVector Direction = ((HitResult.bBlockingHit ? HitResult.ImpactPoint : EndLocation) - GrabbedPrimitive_Temp->GetComponentLocation()).GetSafeNormal();
		const FVector Velocity = Direction * 2750.f;

		GrabbedPrimitive_Temp->SetAllPhysicsLinearVelocity(Velocity);

		AThrowableActor* Throwable = Cast<AThrowableActor>(GrabbedPrimitive_Temp->GetAttachmentRootActor());
		if (IsValid(Throwable))
		{
			Throwable->ThrowSetup(Ability->GetAvatarActorFromActorInfo());
		}
	}

	EndTask();
}