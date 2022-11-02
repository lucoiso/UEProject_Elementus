// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEModularFeatures_ExtraActions

#include "GameFeatureAction_AddAttribute.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Components/GameFrameworkComponentManager.h"
#include "ModularFeatures_InternalFuncs.h"
#include "Runtime/Launch/Resources/Version.h"

void UGameFeatureAction_AddAttribute::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	if (!ensureAlways(ActiveExtensions.IsEmpty()) || !ensureAlways(ActiveRequests.IsEmpty()))
	{
		ResetExtension();
	}

	Super::OnGameFeatureActivating(Context);
}

void UGameFeatureAction_AddAttribute::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);

	ResetExtension();
}

void UGameFeatureAction_AddAttribute::ResetExtension()
{
	while (!ActiveExtensions.IsEmpty())
	{
		const auto ExtensionIterator = ActiveExtensions.CreateConstIterator();
		RemoveAttribute(ExtensionIterator->Key.Get());
	}

	ActiveRequests.Empty();
}

void UGameFeatureAction_AddAttribute::AddToWorld(const FWorldContext& WorldContext)
{
	if (UGameFrameworkComponentManager* const ComponentManager = GetGameFrameworkComponentManager(WorldContext);
		IsValid(ComponentManager) && !TargetPawnClass.IsNull())
	{
		using FHandlerDelegate = UGameFrameworkComponentManager::FExtensionHandlerDelegate;

		const FHandlerDelegate ExtensionHandlerDelegate = FHandlerDelegate::CreateUObject(this, &UGameFeatureAction_AddAttribute::HandleActorExtension);

		ActiveRequests.Add(ComponentManager->AddExtensionHandler(TargetPawnClass, ExtensionHandlerDelegate));
	}
}

void UGameFeatureAction_AddAttribute::HandleActorExtension(AActor* Owner, const FName EventName)
{
	UE_LOG(LogGameplayFeaturesExtraActions, Display, TEXT("Event %s sent by Actor %s for attribute management."), *EventName.ToString(), *Owner->GetName());

	if (EventName == UGameFrameworkComponentManager::NAME_ExtensionRemoved || EventName == UGameFrameworkComponentManager::NAME_ReceiverRemoved)
	{
		RemoveAttribute(Owner);
	}

	else if (EventName == UGameFrameworkComponentManager::NAME_ExtensionAdded || EventName == UGameFrameworkComponentManager::NAME_GameActorReady)
	{
		if (ActiveExtensions.Contains(Owner) || !ActorHasAllRequiredTags(Owner, RequireTags))
		{
			return;
		}

		if (Attribute.IsNull())
		{
			UE_LOG(LogGameplayFeaturesExtraActions, Error, TEXT("%s: Attribute is null."), *FString(__func__));
		}
		else
		{
			AddAttribute(Owner);
		}
	}
}

void UGameFeatureAction_AddAttribute::AddAttribute(AActor* TargetActor)
{
	if (!IsValid(TargetActor) || TargetActor->GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (UAbilitySystemComponent* const AbilitySystemComponent = ModularFeaturesHelper::GetAbilitySystemComponentByActor(TargetActor))
	{
		if (const TSubclassOf<UAttributeSet> SetType = Attribute.LoadSynchronous())
		{
			UAttributeSet* const NewSet = NewObject<UAttributeSet>(AbilitySystemComponent->GetOwnerActor(), SetType);

			if (!InitializationData.IsNull())
			{
				NewSet->InitFromMetaDataTable(InitializationData.LoadSynchronous());
			}

			AbilitySystemComponent->AddAttributeSetSubobject(NewSet);
			AbilitySystemComponent->ForceReplication();

			UE_LOG(LogGameplayFeaturesExtraActions, Display, TEXT("%s: Attribute %s added to Actor %s."), *FString(__func__), *SetType->GetName(), *TargetActor->GetName());

			ActiveExtensions.Add(TargetActor, NewSet);
		}
		else
		{
			UE_LOG(LogGameplayFeaturesExtraActions, Error, TEXT("%s: Attribute is invalid."), *FString(__func__));
		}
	}
	else
	{
		UE_LOG(LogGameplayFeaturesExtraActions, Error, TEXT("%s: Failed to find AbilitySystemComponent on Actor %s."), *FString(__func__), *TargetActor->GetName());
	}
}

void UGameFeatureAction_AddAttribute::RemoveAttribute(AActor* TargetActor)
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

	if (UAbilitySystemComponent* const AbilitySystemComponent = ModularFeaturesHelper::GetAbilitySystemComponentByActor(TargetActor))
	{
#if ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >= 1
		if (UAttributeSet* const AttributeToRemove = ActiveExtensions.FindRef(TargetActor).Get())
		{
			UE_LOG(LogGameplayFeaturesExtraActions, Display, TEXT("%s: Removing attribute %s from Actor %s."), *FString(__func__), *AttributeToRemove->GetName(), *TargetActor->GetName());

			AbilitySystemComponent->RemoveSpawnedAttribute(AttributeToRemove);
			AbilitySystemComponent->ForceReplication();
		}
#elif ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION == 0
		if (UAttributeSet* const AttributeToRemove = ActiveExtensions.FindRef(TargetActor).Get();
			AbilitySystemComponent->GetSpawnedAttributes_Mutable().Remove(AttributeToRemove) != 0)
		{
			UE_LOG(LogGameplayFeaturesExtraActions, Display, TEXT("%s: Attribute %s removed from Actor %s."), *FString(__func__), *AttributeToRemove->GetName(), *TargetActor->GetName());
			AbilitySystemComponent->ForceReplication();
		}
#endif
	}
	else if (IsValid(GetWorld()) && IsValid(GetWorld()->GetGameInstance()))
	{
		UE_LOG(LogGameplayFeaturesExtraActions, Error, TEXT("%s: Failed to find AbilitySystemComponent on Actor %s."), *FString(__func__), *TargetActor->GetName());
	}

	ActiveExtensions.Remove(TargetActor);
}
