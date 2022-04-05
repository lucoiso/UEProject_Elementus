// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEModularFeatures_ExtraActions

#include "GameFeatureAction_AddAttribute.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Components/GameFrameworkComponentManager.h"

void UGameFeatureAction_AddAttribute::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	if (!ensureAlways(ActiveExtensions.IsEmpty()) ||
		!ensureAlways(ActiveRequests.IsEmpty()))
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
					this, &UGameFeatureAction_AddAttribute::HandleActorExtension);

			const TSharedPtr<FComponentRequestHandle> RequestHandle =
				ComponentManager->AddExtensionHandler(TargetPawnClass, ExtensionHandlerDelegate);

			ActiveRequests.Add(RequestHandle);
		}
	}
}

void UGameFeatureAction_AddAttribute::HandleActorExtension(AActor* Owner, FName EventName)
{
	/*UE_LOG(LogGameplayExtraFeatures, Warning,
		   TEXT("Event %s sended by Actor %s for attribute management."), *EventName.ToString(),
		   *Owner->GetActorLabel());*/

	if (EventName == UGameFrameworkComponentManager::NAME_ExtensionRemoved || EventName ==
		UGameFrameworkComponentManager::NAME_ReceiverRemoved)
	{
		RemoveAttribute(Owner);
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

		AddAttribute(Owner);
	}
}

void UGameFeatureAction_AddAttribute::AddAttribute_Implementation(AActor* TargetActor)
{
	if (IsValid(TargetActor) && !Attribute.IsNull() && TargetActor->GetLocalRole() == ROLE_Authority)
	{
		UE_LOG(LogGameplayExtraFeatures, Warning,
			TEXT("Adding attribute %s to Actor %s."), *Attribute.GetAssetName(),
			*TargetActor->GetActorLabel());

		const IAbilitySystemInterface* InterfaceOwner = Cast<IAbilitySystemInterface>(TargetActor);
		UAbilitySystemComponent* AbilitySystemComponent = InterfaceOwner != nullptr
			? InterfaceOwner->GetAbilitySystemComponent()
			: TargetActor->FindComponentByClass<
			UAbilitySystemComponent>();

		if (IsValid(AbilitySystemComponent))
		{
			if (!Attribute.IsNull())
			{
				const TSubclassOf<UAttributeSet> SetType = Attribute.LoadSynchronous();
				if (IsValid(SetType))
				{
					UAttributeSet* NewSet = NewObject<UAttributeSet>(AbilitySystemComponent, SetType);

					if (!InitializationData.IsNull())
					{
						NewSet->InitFromMetaDataTable(InitializationData.LoadSynchronous());
					}

					AbilitySystemComponent->AddAttributeSetSubobject(NewSet);
					AbilitySystemComponent->ForceReplication();

					ActiveExtensions.Add(TargetActor, NewSet);
				}
			}
		}
	}
}

void UGameFeatureAction_AddAttribute::RemoveAttribute_Implementation(AActor* TargetActor)
{
	if (IsValid(TargetActor) && !Attribute.IsNull() && TargetActor->GetLocalRole() == ROLE_Authority)
	{
		UE_LOG(LogGameplayExtraFeatures, Warning,
			TEXT("Removing attribute %s from Actor %s."), *Attribute.GetAssetName(),
			*TargetActor->GetActorLabel());

		const IAbilitySystemInterface* InterfaceOwner = Cast<IAbilitySystemInterface>(TargetActor);
		UAbilitySystemComponent* AbilitySystemComponent = InterfaceOwner != nullptr
			? InterfaceOwner->GetAbilitySystemComponent()
			: TargetActor->FindComponentByClass<
			UAbilitySystemComponent>();

		if (IsValid(AbilitySystemComponent))
		{
			if (!Attribute.IsNull())
			{
				AbilitySystemComponent->GetSpawnedAttributes_Mutable().Remove(
					ActiveExtensions.FindRef(TargetActor).Get());

				AbilitySystemComponent->ForceReplication();

				UE_LOG(LogGameplayExtraFeatures, Warning, TEXT("Attribute %s removed from Actor %s."),
					*Attribute.GetAssetName(), *TargetActor->GetActorLabel());
			}
		}
	}

	ActiveExtensions.Remove(TargetActor);
}