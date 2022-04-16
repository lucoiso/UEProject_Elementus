// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEModularFeatures_ExtraActions

#include "GameFeatureAction_AddEffects.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Components/GameFrameworkComponentManager.h"

void UGameFeatureAction_AddEffects::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	if (!ensureAlways(ActiveExtensions.IsEmpty()) ||
		!ensureAlways(ActiveRequests.IsEmpty()))
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
	const UWorld* World = WorldContext.World();
	const UGameInstance* GameInstance = WorldContext.OwningGameInstance;

	if (IsValid(GameInstance) && IsValid(World) && World->IsGameWorld())
	{
		UGameFrameworkComponentManager* ComponentManager = UGameInstance::GetSubsystem<
			UGameFrameworkComponentManager>(GameInstance);

		if (IsValid(ComponentManager) && !TargetPawnClass.IsNull())
		{
			const UGameFrameworkComponentManager::FExtensionHandlerDelegate ExtensionHandlerDelegate =
				UGameFrameworkComponentManager::FExtensionHandlerDelegate::CreateUObject(
					this, &UGameFeatureAction_AddEffects::HandleActorExtension);

			const TSharedPtr<FComponentRequestHandle> RequestHandle =
				ComponentManager->AddExtensionHandler(TargetPawnClass, ExtensionHandlerDelegate);

			ActiveRequests.Add(RequestHandle);
		}
	}
}

void UGameFeatureAction_AddEffects::HandleActorExtension(AActor* Owner, FName EventName)
{
	/*UE_LOG(LogGameplayFeaturesExtraActions, Display,
		   TEXT("Event %s sended by Actor %s for effects management."), *EventName.ToString(),
		   *Owner->GetName());*/

	if (EventName == UGameFrameworkComponentManager::NAME_ExtensionRemoved || EventName ==
		UGameFrameworkComponentManager::NAME_ReceiverRemoved)
	{
		RemoveEffects(Owner);
	}

	else if (EventName == UGameFrameworkComponentManager::NAME_ExtensionAdded || EventName ==
		UGameFrameworkComponentManager::NAME_GameActorReady)
	{
		if (ActiveExtensions.Contains(Owner))
		{
			return;
		}

		if (RequireTags.Num() != 0)
		{
			for (const FName Tag : RequireTags)
			{
				if (Owner->ActorHasTag(Tag))
				{
					return;
				}
			}
		}

		for (const FEffectStackedData& Entry : Effects)
		{
			if (!Entry.EffectClass.IsNull())
			{
				ApplyEffects(Owner, Entry);
			}
		}
	}
}

void UGameFeatureAction_AddEffects::ApplyEffects(AActor* TargetActor, const FEffectStackedData& Effect)
{
	if (IsValid(TargetActor) && TargetActor->IsActorInitialized() && !Effect.EffectClass.IsNull())
	{
		UE_LOG(LogGameplayFeaturesExtraActions, Display,
			TEXT("Adding effect %s level %u to Actor %s with %u SetByCaller params."),
			*Effect.EffectClass.GetAssetName(), Effect.EffectLevel,
			*TargetActor->GetName(), Effect.SetByCallerParams.Num());

		const IAbilitySystemInterface* InterfaceOwner = Cast<IAbilitySystemInterface>(TargetActor);

		UAbilitySystemComponent* AbilitySystemComponent = InterfaceOwner != nullptr
			? InterfaceOwner->GetAbilitySystemComponent()
			: TargetActor->FindComponentByClass<UAbilitySystemComponent>();

		if (IsValid(AbilitySystemComponent))
		{
			TArray<FActiveGameplayEffectHandle> SpecData = ActiveExtensions.FindOrAdd(TargetActor);

			if (!Effect.EffectClass.IsNull())
			{
				const TSubclassOf<UGameplayEffect> EffectClass = Effect.EffectClass.LoadSynchronous();
				const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
					EffectClass, Effect.EffectLevel, AbilitySystemComponent->MakeEffectContext());

				for (const TPair<FGameplayTag, float> SetByCallerParam : Effect.SetByCallerParams)
				{
					SpecHandle.Data.Get()->SetSetByCallerMagnitude(SetByCallerParam.Key, SetByCallerParam.Value);
				}

				const FActiveGameplayEffectHandle& NewActiveEffect = AbilitySystemComponent->
					ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				SpecData.Add(NewActiveEffect);

				ActiveExtensions.Add(TargetActor, SpecData);
			}
		}
	}
}

void UGameFeatureAction_AddEffects::RemoveEffects(AActor* TargetActor)
{
	if (IsValid(TargetActor))
	{
		UE_LOG(LogGameplayFeaturesExtraActions, Display,
			TEXT("Removing effects from Actor %s."), *TargetActor->GetName());

		TArray<FActiveGameplayEffectHandle> ActiveEffects = ActiveExtensions.FindRef(TargetActor);

		if (!ActiveEffects.IsEmpty())
		{
			const IAbilitySystemInterface* InterfaceOwner = Cast<IAbilitySystemInterface>(TargetActor);

			UAbilitySystemComponent* AbilitySystemComponent = InterfaceOwner != nullptr
				? InterfaceOwner->GetAbilitySystemComponent()
				: TargetActor->FindComponentByClass<UAbilitySystemComponent>();

			if (IsValid(AbilitySystemComponent))
			{
				for (const FActiveGameplayEffectHandle& EffectHandle : ActiveEffects)
				{
					if (EffectHandle.IsValid())
					{
						AbilitySystemComponent->RemoveActiveGameplayEffect(EffectHandle);
					}
				}
			}
		}
	}

	ActiveExtensions.Remove(TargetActor);
}