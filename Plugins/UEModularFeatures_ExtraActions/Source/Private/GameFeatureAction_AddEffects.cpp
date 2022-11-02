// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEModularFeatures_ExtraActions

#include "GameFeatureAction_AddEffects.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Components/GameFrameworkComponentManager.h"
#include "ModularFeatures_InternalFuncs.h"

void UGameFeatureAction_AddEffects::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	if (!ensureAlways(ActiveExtensions.IsEmpty()) || !ensureAlways(ActiveRequests.IsEmpty()))
	{
		ResetExtension();
	}

	Super::OnGameFeatureActivating(Context);
}

void UGameFeatureAction_AddEffects::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);

	ResetExtension();
}

void UGameFeatureAction_AddEffects::ResetExtension()
{
	while (!ActiveExtensions.IsEmpty())
	{
		const auto ExtensionIterator = ActiveExtensions.CreateConstIterator();
		RemoveEffects(ExtensionIterator->Key.Get());
	}

	ActiveRequests.Empty();
}

void UGameFeatureAction_AddEffects::AddToWorld(const FWorldContext& WorldContext)
{
	if (UGameFrameworkComponentManager* const ComponentManager = GetGameFrameworkComponentManager(WorldContext);
		IsValid(ComponentManager) && !TargetPawnClass.IsNull())
	{
		using FHandlerDelegate = UGameFrameworkComponentManager::FExtensionHandlerDelegate;

		const FHandlerDelegate ExtensionHandlerDelegate = FHandlerDelegate::CreateUObject(this, &UGameFeatureAction_AddEffects::HandleActorExtension);

		ActiveRequests.Add(ComponentManager->AddExtensionHandler(TargetPawnClass, ExtensionHandlerDelegate));
	}
}

void UGameFeatureAction_AddEffects::HandleActorExtension(AActor* Owner, const FName EventName)
{
	UE_LOG(LogGameplayFeaturesExtraActions, Display, TEXT("Event %s sent by Actor %s for effects management."), *EventName.ToString(), *Owner->GetName());

	if (EventName == UGameFrameworkComponentManager::NAME_ExtensionRemoved || EventName == UGameFrameworkComponentManager::NAME_ReceiverRemoved)
	{
		RemoveEffects(Owner);
	}

	else if (EventName == UGameFrameworkComponentManager::NAME_ExtensionAdded || EventName == UGameFrameworkComponentManager::NAME_GameActorReady)
	{
		if (ActiveExtensions.Contains(Owner) || !ActorHasAllRequiredTags(Owner, RequireTags))
		{
			return;
		}

		for (const FEffectStackedData& Entry : Effects)
		{
			if (Entry.EffectClass.IsNull())
			{
				UE_LOG(LogGameplayFeaturesExtraActions, Error, TEXT("%s: Effect class is null."), *FString(__func__));
			}
			else
			{
				AddEffects(Owner, Entry);
			}
		}
	}
}

void UGameFeatureAction_AddEffects::AddEffects(AActor* TargetActor, const FEffectStackedData& Effect)
{
	if (!IsValid(TargetActor) || TargetActor->GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (UAbilitySystemComponent* const AbilitySystemComponent = ModularFeaturesHelper::GetAbilitySystemComponentByActor(TargetActor))
	{
		TArray<FActiveGameplayEffectHandle>& SpecData = ActiveExtensions.FindOrAdd(TargetActor);

		const TSubclassOf<UGameplayEffect> EffectClass = Effect.EffectClass.LoadSynchronous();

		UE_LOG(LogGameplayFeaturesExtraActions, Display, TEXT("%s: Adding effect %s level %u to Actor %s with %u SetByCaller params."), *FString(__func__), *EffectClass->GetName(), Effect.EffectLevel, *TargetActor->GetName(), Effect.SetByCallerParams.Num());

		const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(EffectClass, Effect.EffectLevel, AbilitySystemComponent->MakeEffectContext());

		for (const TPair<FGameplayTag, float>& SetByCallerParam : Effect.SetByCallerParams)
		{
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(SetByCallerParam.Key, SetByCallerParam.Value);
		}

		const FActiveGameplayEffectHandle NewActiveEffect = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

		SpecData.Add(NewActiveEffect);

		ActiveExtensions.Add(TargetActor, SpecData);
	}
	else
	{
		UE_LOG(LogGameplayFeaturesExtraActions, Error, TEXT("%s: Failed to find AbilitySystemComponent on Actor %s."), *FString(__func__), *TargetActor->GetName());
	}
}

void UGameFeatureAction_AddEffects::RemoveEffects(AActor* TargetActor)
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

	if (TArray<FActiveGameplayEffectHandle> ActiveEffects = ActiveExtensions.FindRef(TargetActor);
		!ActiveEffects.IsEmpty())
	{
		if (UAbilitySystemComponent* const AbilitySystemComponent = ModularFeaturesHelper::GetAbilitySystemComponentByActor(TargetActor))
		{
			UE_LOG(LogGameplayFeaturesExtraActions, Display, TEXT("%s: Removing effects from Actor %s."), *FString(__func__), *TargetActor->GetName());

			for (const FActiveGameplayEffectHandle& EffectHandle : ActiveEffects)
			{
				if (EffectHandle.IsValid())
				{
					AbilitySystemComponent->RemoveActiveGameplayEffect(EffectHandle);
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
