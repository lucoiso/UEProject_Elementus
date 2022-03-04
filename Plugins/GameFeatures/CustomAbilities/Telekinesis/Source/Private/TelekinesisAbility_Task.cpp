// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "TelekinesisAbility_Task.h"
#include "ThrowableActor.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Actors/Character/PECharacterBase.h"

UTelekinesisAbility_Task::UTelekinesisAbility_Task(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
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

	APECharacterBase* TelekinesisOwner = Cast<APECharacterBase>(GetAvatarActor());
	PhysicsHandle = NewObject<UPhysicsHandleComponent>(TelekinesisOwner, UPhysicsHandleComponent::StaticClass(),
	                                                   FName("TelekinesisPhysicsHandle"));

	if (IsValid(TelekinesisOwner) && IsValid(Cast<AThrowableActor>(TelekinesisTarget)) && PhysicsHandle.IsValid())
	{
		PhysicsHandle->RegisterComponent();
		PhysicsHandle->GrabComponentAtLocation(Cast<UPrimitiveComponent>(TelekinesisTarget->GetRootComponent()),
		                                       NAME_None, TelekinesisTarget->GetActorLocation());
		PhysicsHandle->SetTargetLocation(TelekinesisOwner->GetMesh()->GetSocketLocation("Telekinesis_AbilitySocket"));
	}

	else
	{
		UE_LOG(LogGameplayTasks, Warning, TEXT("Task %s ended"), *GetName());

		bIsFinished = true;
		EndTask();
	}
}

void UTelekinesisAbility_Task::TickTask(const float DeltaTime)
{
	if (bIsFinished)
	{
		EndTask();
		return;
	}

	Super::TickTask(DeltaTime);

	const APECharacterBase* TelekinesisOwner = Cast<APECharacterBase>(GetAvatarActor());

	if (IsValid(TelekinesisOwner) && PhysicsHandle.IsValid() && IsValid(PhysicsHandle->GetGrabbedComponent()))
	{
		PhysicsHandle->SetTargetLocation(TelekinesisOwner->GetMesh()->GetSocketLocation("Telekinesis_AbilitySocket"));
	}

	else
	{
		UE_LOG(LogGameplayTasks, Warning, TEXT("Task %s ended"), *GetName());

		bIsFinished = true;
		EndTask();
	}
}

void UTelekinesisAbility_Task::OnDestroy(const bool AbilityIsEnding)
{
	bIsFinished = true;
	PhysicsHandle->ReleaseComponent();

	Super::OnDestroy(AbilityIsEnding);
}

void UTelekinesisAbility_Task::ThrowObject()
{
	UE_LOG(LogGameplayTasks, Warning, TEXT(" %s called"), __func__);

	bIsFinished = true;

	UPrimitiveComponent* Throwable = PhysicsHandle->GetGrabbedComponent();
	const APECharacterBase* TelekinesisOwner = Cast<APECharacterBase>(GetAvatarActor());

	PhysicsHandle->ReleaseComponent();

	if (IsValid(TelekinesisOwner) && IsValid(Throwable))
	{
		const FVector Velocity = 2750.f * TelekinesisOwner->GetCameraForwardVector();

		Throwable->WakeAllRigidBodies();
		Throwable->SetAllPhysicsLinearVelocity(Velocity);
	}
	else
	{
		UE_LOG(LogGameplayTasks, Warning, TEXT("Task %s ended"), *GetName());
		EndTask();
	}
}
