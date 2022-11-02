// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEModularFeatures_ExtraActions

#include "GameFeatureAction_AddAbilities.h"
#include "AbilityInputBinding.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Engine/AssetManager.h"
#include "InputAction.h"
#include "ModularFeatures_InternalFuncs.h"

void UGameFeatureAction_AddAbilities::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	if (!ensureAlways(ActiveExtensions.IsEmpty()) || !ensureAlways(ActiveRequests.IsEmpty()))
	{
		ResetExtension();
	}

	Super::OnGameFeatureActivating(Context);
}

void UGameFeatureAction_AddAbilities::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);

	ResetExtension();
}

void UGameFeatureAction_AddAbilities::ResetExtension()
{
	while (!ActiveExtensions.IsEmpty())
	{
		const auto ExtensionIterator = ActiveExtensions.CreateConstIterator();
		RemoveActorAbilities(ExtensionIterator->Key.Get());
	}

	ActiveRequests.Empty();
}

void UGameFeatureAction_AddAbilities::AddToWorld(const FWorldContext& WorldContext)
{
	if (UGameFrameworkComponentManager* const ComponentManager = GetGameFrameworkComponentManager(WorldContext);
		IsValid(ComponentManager) && !TargetPawnClass.IsNull())
	{
		using FHandlerDelegate = UGameFrameworkComponentManager::FExtensionHandlerDelegate;

		const FHandlerDelegate ExtensionHandlerDelegate = FHandlerDelegate::CreateUObject(this, &UGameFeatureAction_AddAbilities::HandleActorExtension);

		ActiveRequests.Add(ComponentManager->AddExtensionHandler(TargetPawnClass, ExtensionHandlerDelegate));
	}
}

void UGameFeatureAction_AddAbilities::HandleActorExtension(AActor* Owner, const FName EventName)
{
	UE_LOG(LogGameplayFeaturesExtraActions, Display, TEXT("Event %s sent by Actor %s for ability management."), *EventName.ToString(), *Owner->GetName());

	if (EventName == UGameFrameworkComponentManager::NAME_ExtensionRemoved || EventName == UGameFrameworkComponentManager::NAME_ReceiverRemoved)
	{
		RemoveActorAbilities(Owner);
	}

	else if (EventName == UGameFrameworkComponentManager::NAME_ExtensionAdded || EventName == UGameFrameworkComponentManager::NAME_GameActorReady)
	{
		if (ActiveExtensions.Contains(Owner) || !ActorHasAllRequiredTags(Owner, RequireTags))
		{
			return;
		}

		for (const FAbilityMapping& Entry : Abilities)
		{
			if (Entry.AbilityClass.IsNull())
			{
				UE_LOG(LogGameplayFeaturesExtraActions, Error, TEXT("%s: Ability class is null."), *FString(__func__));
			}
			else
			{
				AddActorAbilities(Owner, Entry);
			}
		}
	}
}

void UGameFeatureAction_AddAbilities::AddActorAbilities(AActor* TargetActor, const FAbilityMapping& Ability)
{
	if (!IsValid(TargetActor) || TargetActor->GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (UAbilitySystemComponent* const AbilitySystemComponent = ModularFeaturesHelper::GetAbilitySystemComponentByActor(TargetActor))
	{
		FActiveAbilityData& NewAbilityData = ActiveExtensions.FindOrAdd(TargetActor);

		const uint32 InputID = InputIDEnumerationClass.LoadSynchronous()->GetValueByName(Ability.InputIDValueName, EGetByNameFlags::CheckAuthoredName);

		const TSubclassOf<UGameplayAbility> AbilityToAdd = Ability.AbilityClass.LoadSynchronous();

		UE_LOG(LogGameplayFeaturesExtraActions, Display, TEXT("%s: Adding ability %s to Actor %s."), *FString(__func__), *AbilityToAdd->GetName(), *TargetActor->GetName());

		const FGameplayAbilitySpec NewAbilitySpec(AbilityToAdd, Ability.AbilityLevel, InputID, TargetActor);

		if (const FGameplayAbilitySpecHandle NewSpecHandle = AbilitySystemComponent->GiveAbility(NewAbilitySpec);
			NewSpecHandle.IsValid())
		{
			NewAbilityData.SpecHandle.Add(NewSpecHandle);

			if (!Ability.InputAction.IsNull())
			{
				if (const IAbilityInputBinding* const SetupInputInterface = ModularFeaturesHelper::GetAbilityInputBindingInterface(TargetActor, InputBindingOwner))
				{
					UInputAction* const AbilityInput = Ability.InputAction.LoadSynchronous();
					IAbilityInputBinding::Execute_SetupAbilityInputBinding(SetupInputInterface->_getUObject(), AbilityInput, InputID);

					NewAbilityData.InputReference.Add(AbilityInput);
				}
				else
				{
					UE_LOG(LogGameplayFeaturesExtraActions, Error, TEXT("%s: Failed to setup input binding for ability %s on Actor %s."), *FString(__func__), *AbilityToAdd->GetName(), *TargetActor->GetName());
				}
			}

			ActiveExtensions.Add(TargetActor, NewAbilityData);
		}
	}
	else
	{
		UE_LOG(LogGameplayFeaturesExtraActions, Error, TEXT("%s: Failed to find AbilitySystemComponent on Actor %s."), *FString(__func__), *TargetActor->GetName());
	}
}

void UGameFeatureAction_AddAbilities::RemoveActorAbilities(AActor* TargetActor)
{
	if (!IsValid(TargetActor))
	{
		ActiveExtensions.Remove(TargetActor);
		return;
	}

	if (TargetActor->GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	const FActiveAbilityData ActiveAbilities = ActiveExtensions.FindRef(TargetActor);

	if constexpr (&ActiveAbilities == nullptr)
	{
		UE_LOG(LogGameplayFeaturesExtraActions, Warning, TEXT("%s: No active abilities found for Actor %s."), *FString(__func__), *TargetActor->GetName());
	}
	else
	{
		if (UAbilitySystemComponent* const AbilitySystemComponent = ModularFeaturesHelper::GetAbilitySystemComponentByActor(TargetActor))
		{
			UE_LOG(LogGameplayFeaturesExtraActions, Display, TEXT("%s: Removing associated abilities from Actor %s."), *FString(__func__), *TargetActor->GetName());

			for (const FGameplayAbilitySpecHandle& SpecHandle : ActiveAbilities.SpecHandle)
			{
				if (SpecHandle.IsValid())
				{
					AbilitySystemComponent->SetRemoveAbilityOnEnd(SpecHandle);
					AbilitySystemComponent->ClearAbility(SpecHandle);
				}
			}

			if (const IAbilityInputBinding* const SetupInputInterface = ModularFeaturesHelper::GetAbilityInputBindingInterface(TargetActor, InputBindingOwner))
			{
				for (const UInputAction* const& InputRef : ActiveAbilities.InputReference)
				{
					if (IsValid(InputRef))
					{
						IAbilityInputBinding::Execute_RemoveAbilityInputBinding(SetupInputInterface->_getUObject(), InputRef);
					}
				}
			}
		}
		else if (IsValid(GetWorld()) && IsValid(GetWorld()->GetGameInstance()))
		{
			UE_LOG(LogGameplayFeaturesExtraActions, Error, TEXT("%s: Failed to find AbilitySystemComponent on Actor %s."), *FString(__func__), *TargetActor->GetName());
		}
	}

	ActiveExtensions.Remove(TargetActor);
}
