// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Actors/Character/PEPlayerController.h"
#include "Actors/Character/PECharacter.h"
#include "Actors/Character/PEPlayerState.h"
#include "PEAbilityTags.h"
#include "Management/Functions/PEEOSLibrary.h"
#include <Management/PEInventorySystemSettings.h>
#include <MFEA_Settings.h>
#include <EnhancedInputComponent.h>
#include <EnhancedPlayerInput.h>
#include <InputAction.h>
#include <AbilitySystemComponent.h>
#include <AbilitySystemGlobals.h>
#include <GameFramework/GameModeBase.h>
#include <GameFramework/PlayerState.h>
#include <Blueprint/UserWidget.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(PEPlayerController)

constexpr float BaseTurnRate = 45.f;
constexpr float BaseLookUpRate = 45.f;

DEFINE_LOG_CATEGORY(LogController_Base);
DEFINE_LOG_CATEGORY(LogController_Axis);

APEPlayerController::APEPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	if (const UMFEA_Settings* MF_Settings = GetDefault<UMFEA_Settings>(); !MF_Settings->InputIDEnumeration.IsNull())
	{
		InputEnumHandle = MF_Settings->InputIDEnumeration.LoadSynchronous();
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
			if (UAbilitySystemComponent* const AbilitySystemComp_Ref = State->GetAbilitySystemComponent())
			{
				AbilitySystemComp_Ref->RemoveActiveEffectsWithTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag(GlobalTag_DeadState)));
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

#pragma region IMFEA_AbilityInputBinding
// Double "_Implementation" because this function is a RPC call version of a virtual function from IAbilityBinding interface
void APEPlayerController::SetupAbilityBindingByInput_Implementation_Implementation(UInputAction* Action, const int32 InputID)
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
#pragma endregion IMFEA_AbilityInputBinding

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
	if (UAbilitySystemComponent* const TargetABSC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetPawn());
		ensureAlwaysMsgf(IsValid(TargetABSC), TEXT("%s owner have a invalid AbilitySystemComponent"), *GetName()))
	{
		// Send the input pressed event to the ability system component with the found input ID
		TargetABSC->AbilityLocalInputPressed(InputID);

		// Verify if the found input ID is equal to Confirm or Cancel input from the specified Enumeration class
		if (ensureAlwaysMsgf(InputEnumHandle.IsValid(), TEXT("%s have a invalid InputEnumHandle"), *GetName()))
		{
			if (InputID == InputEnumHandle->GetValueByName("Confirm", EGetByNameFlags::CheckAuthoredName))
			{
				TargetABSC->LocalInputConfirm();
			}

			else if (InputID == InputEnumHandle->GetValueByName("Cancel", EGetByNameFlags::CheckAuthoredName))
			{
				TargetABSC->LocalInputCancel();
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
	if (UAbilitySystemComponent* const TargetABSC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetPawn());
		ensureAlwaysMsgf(IsValid(TargetABSC), TEXT("%s owner have a invalid AbilitySystemComponent"), *GetName()))
	{
		// Send the input released event to the ability system component with the found input ID
		TargetABSC->AbilityLocalInputReleased(InputID);
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
	const UPEInventorySystemSettings* const InventorySettings = UPEInventorySystemSettings::Get();
	const TSubclassOf<UUserWidget> InventoryUIClass = InventorySettings->MainInventoryWidget.IsNull() ? nullptr : InventorySettings->MainInventoryWidget.LoadSynchronous();
	
	if (!IsValid(InventoryUIClass))
	{
		UE_LOG(LogTemp, Error, TEXT("%s - Missing setting: Main Inventory Widget"), *FString(__func__));
		return;
	}

	CreateWidget(this, InventoryUIClass, TEXT("InventoryWidget"))->AddToPlayerScreen();
}
