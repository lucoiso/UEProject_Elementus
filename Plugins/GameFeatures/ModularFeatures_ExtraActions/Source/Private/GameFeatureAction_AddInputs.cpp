// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEModularFeatures_ExtraActions

#include "GameFeatureAction_AddInputs.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Components/GameFrameworkComponentManager.h"

void UGameFeatureAction_AddInputs::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	if (!ensureAlways(ActiveExtensions.IsEmpty()) ||
		!ensureAlways(ActiveRequests.IsEmpty()))
	{
		ResetExtension();
	}

	Super::OnGameFeatureActivating(Context);
}

void UGameFeatureAction_AddInputs::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);

	ResetExtension();
}

void UGameFeatureAction_AddInputs::ResetExtension()
{
	while (!ActiveExtensions.IsEmpty())
	{
		const auto ExtensionIterator = ActiveExtensions.CreateIterator();
		RemoveActorInputs(ExtensionIterator->Key.Get());
	}

	ActiveRequests.Empty();
}

void UGameFeatureAction_AddInputs::AddToWorld(const FWorldContext& WorldContext)
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
					this, &UGameFeatureAction_AddInputs::HandleActorExtension);

			const TSharedPtr<FComponentRequestHandle> RequestHandle =
				ComponentManager->AddExtensionHandler(TargetPawnClass, ExtensionHandlerDelegate);

			ActiveRequests.Add(RequestHandle);
		}
	}
}

void UGameFeatureAction_AddInputs::HandleActorExtension(AActor* Owner, FName EventName)
{
	/*UE_LOG(LogGameplayExtraFeatures, Warning,
	       TEXT("Event %s sended by Actor %s for ability management."), *EventName.ToString(),
	       *Owner->GetActorLabel());*/

	if (EventName == UGameFrameworkComponentManager::NAME_ExtensionRemoved || EventName ==
		UGameFrameworkComponentManager::NAME_ReceiverRemoved)
	{
		RemoveActorInputs(Owner);
	}

	else if (EventName == UGameFrameworkComponentManager::NAME_ExtensionAdded || EventName ==
		UGameFrameworkComponentManager::NAME_GameActorReady)
	{
		if (!InputMappingContext.IsNull())
		{			
			AddActorInputs(Owner);
		}
	}
}

void UGameFeatureAction_AddInputs::AddActorInputs_Implementation(AActor* TargetActor)
{
	if (IsValid(TargetActor) && TargetActor->IsActorInitialized() && !InputMappingContext.IsNull())
	{
		UE_LOG(LogGameplayExtraFeatures, Warning,
			TEXT("Adding Enhanced Input Mapping %s to Actor %s."), *InputMappingContext.GetAssetName(),
			*TargetActor->GetActorLabel());

		APawn* TargetPawn = Cast<APawn>(TargetActor);
		const APlayerController* PlayerController = TargetPawn->GetController<APlayerController>();

		if (IsValid(PlayerController))
		{
			const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
			if (IsValid(LocalPlayer))
			{
				UEnhancedInputLocalPlayerSubsystem*
					Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

				if (!IsValid(Subsystem))
				{
					return;
				}

				FInputBindingData& NewInputData = ActiveExtensions.FindOrAdd(TargetActor);
				UInputMappingContext* InputMapping = InputMappingContext.LoadSynchronous();

				Subsystem->AddMappingContext(InputMapping, MappingPriority);

				NewInputData.Mapping = InputMapping;

				UObject* FunctionOwner;

				switch (InputBindingOwner)
				{
				case EControllerOwner::Pawn:
					FunctionOwner = TargetPawn;
					break;

				case EControllerOwner::Controller:
					FunctionOwner = TargetPawn->GetController();
					break;

				default:
					FunctionOwner = nullptr;
				}

				UEnhancedInputComponent* InputComponent =
					Cast<UEnhancedInputComponent>(TargetPawn->InputComponent);

				if (IsValid(FunctionOwner) && IsValid(InputComponent))
				{
					for (const FInputMappingStack& InputData : ActionsBindings)
					{
						UE_LOG(LogGameplayExtraFeatures, Warning,
							TEXT("Binding Action Input %s to Actor %s."), *InputData.ActionInput.GetAssetName(),
							*TargetActor->GetActorLabel());

						if (!InputData.ActionInput.IsNull())
						{
							for (const FFunctionStackedData& FunctionData : InputData.FunctionBindingData)
							{
								const UInputAction* InputAction = InputData.ActionInput.LoadSynchronous();

								for (const ETriggerEvent& Trigger : FunctionData.Triggers)
								{
									const FInputBindingHandle& InputBindingHandle = InputComponent->BindAction(
										InputAction, Trigger,
										FunctionOwner, FunctionData.FunctionName);

									NewInputData.ActionBinding.Add(InputBindingHandle);
								}
							}
						}
					}

					ActiveExtensions.Add(TargetActor, NewInputData);
				}
			}
		}
	}
}

void UGameFeatureAction_AddInputs::RemoveActorInputs_Implementation(AActor* TargetActor)
{
	if (IsValid(TargetActor) && !InputMappingContext.IsNull())
	{
		UE_LOG(LogGameplayExtraFeatures, Warning,
		       TEXT("Removing Enhanced Input Mapping %s from Actor %s."), *InputMappingContext.GetAssetName(),
		       *TargetActor->GetActorLabel());

		APawn* TargetPawn = Cast<APawn>(TargetActor);
		const APlayerController* PlayerController = TargetPawn->GetController<APlayerController>();

		if (IsValid(PlayerController))
		{
			const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
			if (IsValid(LocalPlayer))
			{
				UEnhancedInputLocalPlayerSubsystem*
					Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

				if (!IsValid(Subsystem))
				{
					return;
				}

				const FInputBindingData& ActiveInputData = ActiveExtensions.FindRef(TargetActor);
				
#if __cplusplus > 201402L // Detect if compiler version is > c++14
				if constexpr (&ActiveInputData != nullptr)
#else
				if (&ActiveInputData != nullptr)
#endif
				{
					Subsystem->RemoveMappingContext(ActiveInputData.Mapping.Get());

					UObject* FunctionOwner;

					switch (InputBindingOwner)
					{
					case EControllerOwner::Pawn:
						FunctionOwner = TargetPawn;
						break;

					case EControllerOwner::Controller:
						FunctionOwner = TargetPawn->GetController();
						break;

					default:
						FunctionOwner = nullptr;
					}

					UEnhancedInputComponent* InputComponent =
						Cast<UEnhancedInputComponent>(TargetPawn->InputComponent);

					if (IsValid(FunctionOwner) && IsValid(InputComponent))
					{
						for (const FInputBindingHandle& BindHandle : ActiveInputData.ActionBinding)
						{
							InputComponent->RemoveBindingByHandle(BindHandle.GetHandle());
						}
					}
				}
			}
		}
	}

	ActiveExtensions.Remove(TargetActor);
}
