// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Actors/Character/PEPlayerController.h"
#include "Actors/Character/PECharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedPlayerInput.h"
#include "InputAction.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Actors/Character/PEPlayerState.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerState.h"
#include "GAS/System/PEAbilitySystemComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Management/PEGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Components/PEInventoryComponent.h"
#include "ElementusInventoryFunctions.h"
#include "Management/Data/PEGlobalTags.h"
#include "Management/Functions/PEPlayerLibrary.h"

constexpr float BaseTurnRate = 45.f;
constexpr float BaseLookUpRate = 45.f;

DEFINE_LOG_CATEGORY(LogController_Base);
DEFINE_LOG_CATEGORY(LogController_Axis);

APEPlayerController::APEPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	static const ConstructorHelpers::FObjectFinder<UEnum> InputIDEnum_ObjRef(TEXT("/Game/Main/Data/GAS/EN_AbilityInputID"));
	if (InputIDEnum_ObjRef.Succeeded())
	{
		InputEnumHandle = InputIDEnum_ObjRef.Object;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> InventoryWidget_ClassRef(TEXT("/Game/Main/Blueprints/Widgets/Inventory/WB_Inventory_Example"));
	if (InventoryWidget_ClassRef.Succeeded())
	{
		InventoryWidgetClass = InventoryWidget_ClassRef.Class;
	}
}

void APEPlayerController::SetupControllerSpectator_Implementation()
{
	ChangeState(NAME_Spectating);
	PlayerState->SetIsSpectator(true);
}

void APEPlayerController::InitializeRespawn(const float InSeconds)
{
	if (IsInState(NAME_Spectating))
	{
		// If InSeconds is 0, then we want to respawn instantly
		if (InSeconds > 0.f)
		{
			FTimerDelegate TimerDelegate;
			TimerDelegate.BindLambda([&]
			{
				if (IsValid(this))
				{
					RespawnAndPossess();
				}
			});

			FTimerHandle Handle;
			GetWorld()->GetTimerManager().SetTimer(Handle, TimerDelegate, InSeconds, false);
		}
		else
		{
			RespawnAndPossess();
		}
	}
}

void APEPlayerController::RespawnAndPossess_Implementation()
{
	if (const AActor* const PlayerStart = GetWorld()->GetAuthGameMode()->FindPlayerStart(this))
	{
		if (const APEPlayerState* const State = GetPlayerState<APEPlayerState>())
		{
			if (UPEAbilitySystemComponent* AbilitySystemComp_Ref = CastChecked<UPEAbilitySystemComponent>(State->GetAbilitySystemComponent()))
			{
				AbilitySystemComp_Ref->RemoveActiveEffectsWithTags(FGameplayTagContainer(GlobalTag_DeadState));
			}
		}

		const FVector RespawnLocation = PlayerStart->GetActorLocation();
		const FRotator RespawnRotation = PlayerStart->GetActorRotation();

		if (APECharacter* const SpawnedCharacter_Ref = GetWorld()->SpawnActor<APECharacter>(RespawnLocation, RespawnRotation))
		{
			Possess(SpawnedCharacter_Ref);
			ChangeState(NAME_Playing);
			PlayerState->SetIsSpectator(false);
		}
	}
}

void APEPlayerController::ProcessGameplayEffect(const TSubclassOf<UGameplayEffect> EffectClass)
{
	Server_ProcessGEApplication_Internal(EffectClass);
}

void APEPlayerController::Server_ProcessGEApplication_Internal_Implementation(const TSubclassOf<UGameplayEffect> EffectClass)
{
	if (!HasAuthority())
	{
		return;
	}

	if (UAbilitySystemComponent* const TargetABSC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetPawn()))
	{
		TargetABSC->ApplyGameplayEffectToSelf(EffectClass.GetDefaultObject(), 1.f, TargetABSC->MakeEffectContext());
	}
}

#pragma region Elementus Inventory Trade
void APEPlayerController::ProcessTrade(const TArray<FElementusItemInfo>& TradeInfo, UElementusInventoryComponent* OtherComponent, const bool bIsFromPlayer)
{
	if (HasAuthority())
	{
		ProcessTrade_Internal(TradeInfo, OtherComponent, bIsFromPlayer);
	}
	else
	{
		Server_ProcessTrade_Internal(TradeInfo, OtherComponent, bIsFromPlayer);
	}
}

void APEPlayerController::Server_ProcessTrade_Internal_Implementation(const TArray<FElementusItemInfo>& TradeInfo, UElementusInventoryComponent* OtherComponent, const bool bIsFromPlayer)
{
	ProcessTrade_Internal(TradeInfo, OtherComponent, bIsFromPlayer);
}

void APEPlayerController::ProcessTrade_Internal(const TArray<FElementusItemInfo>& TradeInfo, UElementusInventoryComponent* OtherComponent, const bool bIsFromPlayer) const
{
	if (!HasAuthority())
	{
		return;
	}

	if (const APECharacter* const ControllerOwner = GetPawn<APECharacter>();
		ensureAlwaysMsgf(IsValid(ControllerOwner), TEXT("%s have a invalid character."), *GetName()))
	{
		if (UPEInventoryComponent* const OwningInventory = ControllerOwner->GetInventoryComponent();
			ensureAlwaysMsgf(IsValid(OwningInventory), TEXT("%s have a invalid inventory."), *GetName()))
		{
			if (bIsFromPlayer && OtherComponent == nullptr)
			{
				OwningInventory->UpdateElementusItems(TradeInfo, EElementusInventoryUpdateOperation::Remove);
			}
			else
			{
				bIsFromPlayer ? UElementusInventoryFunctions::TradeElementusItem(TradeInfo, OwningInventory, OtherComponent)
					: UElementusInventoryFunctions::TradeElementusItem(TradeInfo, OtherComponent, OwningInventory);
			}
		}
	}
}
#pragma endregion Elementus Inventory Trade

#pragma region IAbilityInputBinding
// Double "_Implementation" because this function is a RPC call version of a virtual function from IAbilityBinding interface
void APEPlayerController::SetupAbilityInputBinding_Implementation_Implementation(UInputAction* Action, const int32 InputID)
{
	if (!IsValid(Action))
	{
		CONTROLLER_BASE_VLOG(this, Error, TEXT("%s - Failed to bind ability input for %s with id %d due to invalid Action"), *FString(__func__), *GetName(), InputID);
		return;
	}

	CONTROLLER_BASE_VLOG(this, Display, TEXT("%s - Setting up ability input binding for %s with action %s and id %u"), *FString(__func__), *GetName(), *Action->GetName(), InputID);

	if (UEnhancedInputComponent* const EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
		ensureAlwaysMsgf(IsValid(EnhancedInputComponent), TEXT("%s have a invalid EnhancedInputComponent"), *GetName()))
	{
		const FAbilityInputData AbilityBinding
		{
			EnhancedInputComponent->BindAction(Action, ETriggerEvent::Started, this, &APEPlayerController::OnAbilityInputPressed, Action).GetHandle(),
			EnhancedInputComponent->BindAction(Action, ETriggerEvent::Completed, this, &APEPlayerController::OnAbilityInputReleased, Action).GetHandle(),
			static_cast<uint32>(InputID)
		};

		AbilityActionBindings.Add(Action, AbilityBinding);
	}
}

// Double "_Implementation" because this function is a RPC call version of a virtual function from IAbilityBinding interface
void APEPlayerController::RemoveAbilityInputBinding_Implementation_Implementation(const UInputAction* Action)
{
	CONTROLLER_BASE_VLOG(this, Display, TEXT("%s - Removing ability input binding for %s with action %s"), *FString(__func__), *GetName(), IsValid(Action) ? *Action->GetName() : *FString("NULL"));
	
	if (UEnhancedInputComponent* const EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
		ensureAlwaysMsgf(IsValid(EnhancedInputComponent), TEXT("%s have a invalid EnhancedInputComponent"), *GetName()))
	{
		EnhancedInputComponent->RemoveBindingByHandle(AbilityActionBindings.FindRef(Action).OnPressedHandle);
		EnhancedInputComponent->RemoveBindingByHandle(AbilityActionBindings.FindRef(Action).OnReleasedHandle);

		AbilityActionBindings.Remove(Action);
	}
}
#pragma endregion IAbilityInputBinding

void APEPlayerController::OnAbilityInputPressed(UInputAction* SourceAction)
{
	if (!IsValid(GetPawn()))
	{
		CONTROLLER_BASE_VLOG(this, Warning, TEXT("%s called with invalid Pawn"), *FString(__func__));
		return;
	}

	if (!IsValid(SourceAction))
	{
		CONTROLLER_BASE_VLOG(this, Warning, TEXT("%s called with invalid Action"), *FString(__func__));
		return;
	}

	const uint32 InputID = AbilityActionBindings.FindRef(SourceAction).InputID;

	CONTROLLER_BASE_VLOG(this, Display, TEXT("%s called with Action %s and Input ID Value %u"), *FString(__func__), *SourceAction->GetName(), InputID);

	// Check if controller owner is valid and owns a ability system component
	if (const APECharacter* const ControllerOwner = GetPawn<APECharacter>();
		ensureAlwaysMsgf(IsValid(ControllerOwner->GetAbilitySystemComponent()), TEXT("%s owner have a invalid AbilitySystemComponent"), *GetName()))
	{
		// Send the input pressed event to the ability system component with the found input ID
		ControllerOwner->GetAbilitySystemComponent()->AbilityLocalInputPressed(InputID);

		// Verify if the found input ID is equal to Confirm or Cancel input from the specified Enumeration class
		if (ensureAlwaysMsgf(InputEnumHandle.IsValid(), TEXT("%s have a invalid InputEnumHandle"), *GetName()))
		{
			if (InputID == InputEnumHandle->GetValueByName("Confirm", EGetByNameFlags::CheckAuthoredName))
			{
				ControllerOwner->GetAbilitySystemComponent()->LocalInputConfirm();
			}

			else if (InputID == InputEnumHandle->GetValueByName("Cancel", EGetByNameFlags::CheckAuthoredName))
			{
				ControllerOwner->GetAbilitySystemComponent()->LocalInputCancel();
			}
		}
	}
}

void APEPlayerController::OnAbilityInputReleased(UInputAction* SourceAction)
{
	if (!IsValid(GetPawn()))
	{
		CONTROLLER_BASE_VLOG(this, Warning, TEXT("%s called with invalid Pawn"), *FString(__func__));
		return;
	}

	if (!IsValid(SourceAction))
	{
		CONTROLLER_BASE_VLOG(this, Warning, TEXT("%s called with invalid Action"), *FString(__func__));
		return;
	}

	const uint32 InputID = AbilityActionBindings.FindRef(SourceAction).InputID;

	CONTROLLER_BASE_VLOG(this, Display, TEXT("%s called with Action %s and Input ID Value %u"), *FString(__func__), *SourceAction->GetName(), InputID);

	// Check if controller owner is valid and owns a ability system component
	if (const APECharacter* const ControllerOwner = GetPawn<APECharacter>();
		ensureAlwaysMsgf(IsValid(ControllerOwner->GetAbilitySystemComponent()), TEXT("%s owner have a invalid AbilitySystemComponent"), *GetName()))
	{
		// Send the input released event to the ability system component with the found input ID
		ControllerOwner->GetAbilitySystemComponent()->AbilityLocalInputReleased(InputID);
	}
}

void APEPlayerController::SetVoiceChatEnabled(const FInputActionValue& Value) const
{
	CONTROLLER_BASE_VLOG(this, Display, TEXT("%s called with Input Action Value %s (magnitude %f)"), *FString(__func__), *Value.ToString(), Value.GetMagnitude());

	// [EOS] Call the static function responsible for activating/deactivating the voice chat
	UPEEOSLibrary::MuteEOSSessionVoiceChatUser(NetPlayerIndex, !Value.Get<bool>());
}

void APEPlayerController::OpenInventory(const FInputActionValue& Value)
{
	CONTROLLER_BASE_VLOG(this, Display, TEXT("%s called with Input Action Value %s (magnitude %f)"), *FString(__func__), *Value.ToString(), Value.GetMagnitude());

	Client_OpenInventory();
}

#pragma region Default Movement Functions
void APEPlayerController::ChangeCameraAxis(const FInputActionValue& Value)
{
	if (!IsValid(GetPawnOrSpectator()))
	{
		CONTROLLER_AXIS_VLOG(this, Warning, TEXT("%s called with invalid Pawn"), *FString(__func__));
		return;
	}

	CONTROLLER_AXIS_VLOG(this, Display, TEXT("%s called with Input Action Value %s (magnitude %f)"), *FString(__func__), *Value.ToString(), Value.GetMagnitude());

	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddYawInput(LookAxisVector.X);
	AddPitchInput(LookAxisVector.Y);
}

void APEPlayerController::Move(const FInputActionValue& Value) const
{
	if (!IsValid(GetPawnOrSpectator()))
	{
		CONTROLLER_AXIS_VLOG(this, Warning, TEXT("%s called with invalid Pawn"), *FString(__func__));
		return;
	}

	CONTROLLER_AXIS_VLOG(this, Display, TEXT("%s called with Input Action Value %s (magnitude %f)"), *FString(__func__), *Value.ToString(), Value.GetMagnitude());

	if (Value.GetMagnitude() != 0.f && !IsMoveInputIgnored())
	{
		const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);

		const FVector DirectionX = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector DirectionY = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		const FVector2D MovementVector = Value.Get<FVector2D>();
		
		GetPawnOrSpectator()->AddMovementInput(DirectionX, MovementVector.X);
		GetPawnOrSpectator()->AddMovementInput(DirectionY, MovementVector.Y);

		UE_VLOG_LOCATION(GetPawn(), LogController_Axis, Log, GetPawn()->GetActorLocation(), 25.f, FColor::Green, TEXT("%s"), *GetPawn()->GetName());
	}
}

void APEPlayerController::Jump(const FInputActionValue& Value) const
{
	if (!IsValid(GetPawn()))
	{
		CONTROLLER_AXIS_VLOG(this, Warning, TEXT("%s called with invalid Pawn"), *FString(__func__));
		return;
	}

	CONTROLLER_BASE_VLOG(this, Display, TEXT("%s called with Input Action Value %s (magnitude %f)"), *FString(__func__), *Value.ToString(), Value.GetMagnitude());

	if (APECharacter* const ControllerOwner = GetPawn<APECharacter>())
	{
		Value.Get<bool>() ? ControllerOwner->Jump() : ControllerOwner->StopJumping();
	}
}
#pragma endregion Default Movement Functions

void APEPlayerController::Client_OpenInventory_Implementation()
{
	CreateWidget(this, InventoryWidgetClass.LoadSynchronous(), TEXT("InventoryWidget"))->AddToViewport();
}
