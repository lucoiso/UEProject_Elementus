// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Tasks/PEInteractAbility_Task.h"

#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbilityTargetDataFilter.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayTag.h"
#include "Actors/Interfaces/PEInteractable.h"
#include "GAS/Targeting/PELineTargeting.h"

UPEInteractAbility_Task::UPEInteractAbility_Task(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
	bIsFinished = false;
}

UPEInteractAbility_Task* UPEInteractAbility_Task::InteractionTask(UGameplayAbility* OwningAbility,
                                                                  const FName TaskInstanceName,
                                                                  const float InteractionRange)
{
	UPEInteractAbility_Task* MyObj = NewAbilityTask<UPEInteractAbility_Task>(OwningAbility, TaskInstanceName);
	MyObj->Range = InteractionRange;
	return MyObj;
}

void UPEInteractAbility_Task::Activate()
{
	Super::Activate();

	if (ensureMsgf(IsValid(Ability), TEXT("%s have a invalid Ability"), *GetName()))
	{
		InteractionOwner = Cast<APECharacter>(Ability->GetAvatarActorFromActorInfo());

		if (ensureMsgf(InteractionOwner.IsValid(), TEXT("%s have a invalid Owner"), *GetName()))
		{
			UAbilityTask_WaitGameplayTagAdded* WaitGameplayTagAdd =
				UAbilityTask_WaitGameplayTagAdded::WaitGameplayTagAdd(Ability,
				                                                      FGameplayTag::RequestGameplayTag(
					                                                      FName("State.CannotInteract")));

			WaitGameplayTagAdd->Added.AddDynamic(this, &UPEInteractAbility_Task::OnCannotInteractChanged);

			UAbilityTask_WaitGameplayTagRemoved* WaitGameplayTagRemove =
				UAbilityTask_WaitGameplayTagRemoved::WaitGameplayTagRemove(Ability,
				                                                           FGameplayTag::RequestGameplayTag(
					                                                           FName("State.CannotInteract")));

			WaitGameplayTagRemove->Removed.AddDynamic(this, &UPEInteractAbility_Task::OnCannotInteractChanged);

			WaitGameplayTagAdd->ReadyForActivation();
			WaitGameplayTagRemove->ReadyForActivation();

			return;
		}
	}

	bIsFinished = true;
	EndTask();
}

bool UPEInteractAbility_Task::GetIsInteractAllowed() const
{
	return AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("State.CanInteract")) &&
		!AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("State.CannotInteract"));
}

AActor* UPEInteractAbility_Task::GetInteractable() const
{
	return LastInteractable_Ref.Get();
}

void UPEInteractAbility_Task::OnCannotInteractChanged()
{
	bTickingTask = !AbilitySystemComponent->HasMatchingGameplayTag(
		FGameplayTag::RequestGameplayTag(FName("State.CannotInteract")));
}

void UPEInteractAbility_Task::TickTask(const float DeltaTime)
{
	if (bIsFinished)
	{
		EndTask();
		return;
	}

	Super::TickTask(DeltaTime);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Ability->GetAvatarActorFromActorInfo());

	FVector StartLocation = InteractionOwner->GetCameraComponentLocation();
	FVector EndLocation = StartLocation + InteractionOwner->GetCameraForwardVector() * Range;

	FHitResult HitResult;
	FGameplayTargetDataFilterHandle DataFilterHandle;

	APELineTargeting::LineTraceWithFilter(HitResult, GetWorld(), DataFilterHandle, StartLocation,
	                                      EndLocation, "BlockAll", QueryParams);

	if (IsValid(HitResult.GetActor()) && HitResult.GetActor()->Implements<UPEInteractable>())
	{
		if (LastInteractable_Ref.Get() != HitResult.GetActor())
		{
			if (LastInteractable_Ref.IsValid())
			{
				IPEInteractable* OldInteractable = Cast<IPEInteractable>(LastInteractable_Ref.Get());
				IPEInteractable::Execute_SetIsCurrentlyFocusedByActor(OldInteractable->_getUObject(), false,
				                                                      InteractionOwner.Get());
				LastInteractable_Ref.Reset();
			}

			LastInteractable_Ref = HitResult.GetActor();
			IPEInteractable::Execute_SetIsCurrentlyFocusedByActor(LastInteractable_Ref.Get(), true,
			                                                      InteractionOwner.Get());

			AbilitySystemComponent->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag("State.CanInteract"));
		}
	}
	else
	{
		if (AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("State.CanInteract")))
		{
			AbilitySystemComponent->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("State.CanInteract"));
		}

		if (LastInteractable_Ref.IsValid())
		{
			IPEInteractable::Execute_SetIsCurrentlyFocusedByActor(LastInteractable_Ref.Get(), false,
			                                                      InteractionOwner.Get());
			LastInteractable_Ref.Reset();
		}
	}
}

void UPEInteractAbility_Task::OnDestroy(const bool AbilityIsEnding)
{
	UE_LOG(LogGameplayTasks, Display, TEXT("Task %s ended"), *GetName());

	bIsFinished = true;

	Super::OnDestroy(AbilityIsEnding);
}
