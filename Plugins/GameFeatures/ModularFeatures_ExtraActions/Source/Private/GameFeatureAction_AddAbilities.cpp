// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEModularFeatures_ExtraActions

#include "GameFeatureAction_AddAbilities.h"
#include "AbilityInputBinding.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameFeaturesSubsystemSettings.h"
#include "Engine/AssetManager.h"
#include "InputAction.h"

void UGameFeatureAction_AddAbilities::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	if (!ensureAlways(ActiveExtensions.IsEmpty()) ||
		!ensureAlways(ActiveRequests.IsEmpty()))
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
		const auto ExtensionIterator = ActiveExtensions.CreateIterator();
		RemoveActorAbilities(ExtensionIterator->Key.Get());
	}

	ActiveRequests.Empty();
}

void UGameFeatureAction_AddAbilities::AddToWorld(const FWorldContext& WorldContext)
{
	const UWorld* World = WorldContext.World();
	const UGameInstance* GameInstance = WorldContext.OwningGameInstance;

	if (IsValid(GameInstance) && IsValid(World) && World->IsGameWorld())
	{
		UGameFrameworkComponentManager* ComponentManager = UGameInstance::GetSubsystem<
			UGameFrameworkComponentManager>(GameInstance);

		if (IsValid(ComponentManager) && !TargetActorClass.IsNull())
		{
			const UGameFrameworkComponentManager::FExtensionHandlerDelegate ExtensionHandlerDelegate =
				UGameFrameworkComponentManager::FExtensionHandlerDelegate::CreateUObject(
					this, &UGameFeatureAction_AddAbilities::HandleActorExtension);

			const TSharedPtr<FComponentRequestHandle> RequestHandle =
				ComponentManager->AddExtensionHandler(TargetActorClass, ExtensionHandlerDelegate);

			ActiveRequests.Add(RequestHandle);
		}
	}
}

void UGameFeatureAction_AddAbilities::HandleActorExtension(AActor* Owner, FName EventName)
{
	/*UE_LOG(LogGameplayExtraFeatures, Warning,
		   TEXT("Event %s sended by Actor %s for ability management."), *EventName.ToString(),
		   *Owner->GetActorLabel());*/

	if (EventName == UGameFrameworkComponentManager::NAME_ExtensionRemoved || EventName ==
		UGameFrameworkComponentManager::NAME_ReceiverRemoved)
	{
		RemoveActorAbilities(Owner);
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

		for (const FAbilityMapping& Entry : Abilities)
		{
			if (!Entry.AbilityClass.IsNull())
			{
				AddActorAbilities(Owner, Entry);
			}
		}
	}
}

void UGameFeatureAction_AddAbilities::AddActorAbilities_Implementation(AActor* TargetActor,
	const FAbilityMapping& Ability)
{
	if (IsValid(TargetActor) && TargetActor->IsActorInitialized() && TargetActor->GetLocalRole() == ROLE_Authority)
	{
		UE_LOG(LogGameplayExtraFeatures, Warning,
			TEXT("Adding ability %s to Actor %s."), *Ability.AbilityClass.GetAssetName(),
			*TargetActor->GetActorLabel());

		const IAbilitySystemInterface* InterfaceOwner = Cast<IAbilitySystemInterface>(TargetActor);
		UAbilitySystemComponent* AbilitySystemComponent = InterfaceOwner != nullptr
			? InterfaceOwner->GetAbilitySystemComponent()
			: TargetActor->FindComponentByClass<
			UAbilitySystemComponent>();

		if (IsValid(AbilitySystemComponent))
		{
			FActiveAbilityData NewAbilityData = ActiveExtensions.FindOrAdd(TargetActor);

			const uint32 InputID = InputIDEnumerationClass.LoadSynchronous()->GetValueByName(
				Ability.InputIDValueName, EGetByNameFlags::CheckAuthoredName);

			const FGameplayAbilitySpec NewAbilitySpec =
				FGameplayAbilitySpec(Ability.AbilityClass.LoadSynchronous(),
					Ability.AbilityLevel,
					InputID,
					TargetActor);

			const FGameplayAbilitySpecHandle NewSpecHandle = AbilitySystemComponent->GiveAbility(NewAbilitySpec);

			if (NewSpecHandle.IsValid())
			{
				NewAbilityData.SpecHandle.Add(NewSpecHandle);

				if (!Ability.InputAction.IsNull())
				{
					IAbilityInputBinding* SetupInputInterface;
					APawn* TargetPawn = Cast<APawn>(AbilitySystemComponent->GetAvatarActor());

					switch (InputBindingOwner)
					{
					case EControllerOwner::Pawn:
						SetupInputInterface = Cast<IAbilityInputBinding>(TargetPawn);
						break;

					case EControllerOwner::Controller:
						SetupInputInterface = Cast<IAbilityInputBinding>(TargetPawn->GetController<AController>());
						break;

					default:
						SetupInputInterface = nullptr;
					}

					if (SetupInputInterface != nullptr)
					{
						UInputAction* AbilityInput = Ability.InputAction.LoadSynchronous();
						SetupInputInterface->SetupAbilityInput(AbilityInput,
							InputID);

						NewAbilityData.InputReference.Add(AbilityInput);
					}
				}

				ActiveExtensions.Add(TargetActor, NewAbilityData);
			}
		}
	}
}

void UGameFeatureAction_AddAbilities::RemoveActorAbilities_Implementation(AActor* TargetActor)
{
	if (IsValid(TargetActor) && TargetActor->GetLocalRole() == ROLE_Authority)
	{
		UE_LOG(LogGameplayExtraFeatures, Warning,
			TEXT("Removing associated abilities from Actor %s."),
			*TargetActor->GetActorLabel());

		const FActiveAbilityData ActiveAbilities = ActiveExtensions.FindRef(TargetActor);

#if __cplusplus > 201402L // Detect if compiler version is > c++14
		if constexpr (&ActiveAbilities != nullptr)
#else
		if (&ActiveAbilities != nullptr)
#endif
		{
			const IAbilitySystemInterface* InterfaceOwner = Cast<IAbilitySystemInterface>(TargetActor);
			UAbilitySystemComponent* AbilitySystemComponent = InterfaceOwner != nullptr
				? InterfaceOwner->GetAbilitySystemComponent()
				: TargetActor->FindComponentByClass<UAbilitySystemComponent>();

			if (IsValid(AbilitySystemComponent))
			{
				for (const FGameplayAbilitySpecHandle& SpecHandle : ActiveAbilities.SpecHandle)
				{
					if (SpecHandle.IsValid())
					{
						AbilitySystemComponent->SetRemoveAbilityOnEnd(SpecHandle);
					}
				}

				IAbilityInputBinding* SetupInputInterface;
				APawn* TargetPawn = Cast<APawn>(AbilitySystemComponent->GetAvatarActor());

				switch (InputBindingOwner)
				{
				case EControllerOwner::Pawn:
					SetupInputInterface = Cast<IAbilityInputBinding>(TargetPawn);
					break;

				case EControllerOwner::Controller:
					SetupInputInterface = Cast<IAbilityInputBinding>(TargetPawn->GetController<AController>());
					break;

				default:
					SetupInputInterface = nullptr;
				}

				if (SetupInputInterface != nullptr)
				{
					for (const UInputAction* InputRef : ActiveAbilities.InputReference)
					{
						if (IsValid(InputRef)) 
						{
							SetupInputInterface->RemoveAbilityInputBinding(InputRef);
						}
					}
				}
			}
		}
	}

	ActiveExtensions.Remove(TargetActor);
}