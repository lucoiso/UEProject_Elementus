// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Actors/Character/PEPlayerController.h"
#include "Actors/Character/PECharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedPlayerInput.h"
#include "InputAction.h"
#include "AbilitySystemComponent.h"
#include "ElementusInventoryComponent.h"
#include "ElementusInventoryFunctions.h"
#include "Actors/Character/PEPlayerState.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerState.h"
#include "GAS/System/PEAbilitySystemComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Management/PEGameInstance.h"

constexpr float BaseTurnRate = 45.f;
constexpr float BaseLookUpRate = 45.f;

DEFINE_LOG_CATEGORY(LogController_Base);
DEFINE_LOG_CATEGORY(LogController_Axis);

APEPlayerController::APEPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	static const ConstructorHelpers::FObjectFinder<UEnum> InputIDEnum_ObjRef(
		TEXT("/Game/Main/Data/GAS/EN_AbilityInputID"));
	if constexpr (&InputIDEnum_ObjRef.Object != nullptr)
	{
		InputEnumHandle = InputIDEnum_ObjRef.Object;
	}
}

void APEPlayerController::InitializeRespawn(const float InSeconds)
{
	if (IsInState(NAME_Spectating))
	{
		// If InSeconds is 0, then we want to respawn instantly
		if (InSeconds > 0.f)
		{
			FTimerDelegate TimerDelegate;
			TimerDelegate.BindLambda([this]
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

#pragma region Elementus Inventory Trade

#define TRADE_ELEMENTUS_ITEM(ItemInfo, OtherComponent, bIsFromPlayer) \
if (const APECharacter* ControllerOwner = GetPawn<APECharacter>(); \
ensureAlwaysMsgf(ControllerOwner->InventoryComponent, \
TEXT("%s owner have a invalid InventoryComponent"), *GetName())) \
{ \
bIsFromPlayer \
? UElementusInventoryFunctions::TradeElementusItem(ItemInfo, ControllerOwner->InventoryComponent, OtherComponent) \
: UElementusInventoryFunctions::TradeElementusItem(ItemInfo, OtherComponent, ControllerOwner->InventoryComponent); \
}

void APEPlayerController::ProcessTrade(const FElementusItemInfo ItemInfo,
                                       UElementusInventoryComponent* OtherComponent,
                                       const bool bIsFromPlayer)
{
	if (HasAuthority())
	{
		TRADE_ELEMENTUS_ITEM(ItemInfo, OtherComponent, bIsFromPlayer);
	}
	else
	{
		Server_ProcessTrade(ItemInfo, OtherComponent, bIsFromPlayer);
	}
}

void APEPlayerController::Server_ProcessTrade_Implementation(const FElementusItemInfo ItemInfo,
                                                             UElementusInventoryComponent* OtherComponent,
                                                             const bool bIsFromPlayer)
{
	TRADE_ELEMENTUS_ITEM(ItemInfo, OtherComponent, bIsFromPlayer);
}
#undef TRADE_ELEMENTUS_ITEM
#pragma endregion Elementus Inventory Trade

void APEPlayerController::RespawnAndPossess_Implementation()
{
	if (const AActor* PlayerStart = GetWorld()->GetAuthGameMode()->FindPlayerStart(this))
	{
		if (const APEPlayerState* State = GetPlayerState<APEPlayerState>())
		{
			if (UPEAbilitySystemComponent* AbilitySystemComp_Ref =
				CastChecked<UPEAbilitySystemComponent>(State->GetAbilitySystemComponent()))
			{
				AbilitySystemComp_Ref->RemoveActiveEffectsWithTags(
					FGameplayTagContainer(FGameplayTag::RequestGameplayTag(TEXT("State.Dead"))));
			}
		}

		const FVector RespawnLocation = PlayerStart->GetActorLocation();
		const FRotator RespawnRotation = PlayerStart->GetActorRotation();

		if (APECharacter* SpawnedCharacter_Ref =
			GetWorld()->SpawnActor<APECharacter>(RespawnLocation, RespawnRotation))
		{
			Possess(SpawnedCharacter_Ref);
			ChangeState(NAME_Playing);
			PlayerState->SetIsSpectator(false);
		}
	}
}

void APEPlayerController::SetupControllerSpectator_Implementation()
{
	ChangeState(NAME_Spectating);
	PlayerState->SetIsSpectator(true);
}

#pragma region IAbilityInputBinding
// Double "_Implementation" because this function is a RPC call version of a virtual function from IAbilityBinding interface
void APEPlayerController::SetupAbilityInputBinding_Implementation_Implementation(UInputAction* Action,
	const int32 InputID)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
		ensureAlwaysMsgf(IsValid(EnhancedInputComponent), TEXT("%s have a invalid EnhancedInputComponent"), *GetName()))
	{
		const FAbilityInputData AbilityBinding
		{
			EnhancedInputComponent->BindAction(Action, ETriggerEvent::Started, this,
			                                   &APEPlayerController::OnAbilityInputPressed, Action).GetHandle(),
			EnhancedInputComponent->BindAction(Action, ETriggerEvent::Completed, this,
			                                   &APEPlayerController::OnAbilityInputReleased, Action).GetHandle(),
			static_cast<uint32>(InputID)
		};

		AbilityActionBindings.Add(Action, AbilityBinding);
	}
}

// Double "_Implementation" because this function is a RPC call version of a virtual function from IAbilityBinding interface
void APEPlayerController::RemoveAbilityInputBinding_Implementation_Implementation(const UInputAction* Action) const
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
		ensureAlwaysMsgf(IsValid(EnhancedInputComponent), TEXT("%s have a invalid EnhancedInputComponent"), *GetName()))
	{
		EnhancedInputComponent->RemoveBindingByHandle(AbilityActionBindings.FindRef(Action).OnPressedHandle);
		EnhancedInputComponent->RemoveBindingByHandle(AbilityActionBindings.FindRef(Action).OnReleasedHandle);
	}
}
#pragma endregion IAbilityInputBinding

void APEPlayerController::OnAbilityInputPressed(UInputAction* Action) const
{
	if (!IsValid(GetPawn()))
	{
		CONTROLLER_BASE_VLOG(this, Warning, TEXT(" %s called with invalid Pawn"), *FString(__func__));
		return;
	}

	const uint32 InputID = AbilityActionBindings.FindRef(Action).InputID;

	CONTROLLER_BASE_VLOG(this, Display, TEXT(" %s called with Input ID Value %u"), *FString(__func__), InputID);

	// Check if controller owner is valid and owns a ability system component
	if (const APECharacter* ControllerOwner = GetPawn<APECharacter>();
		ensureAlwaysMsgf(IsValid(ControllerOwner->GetAbilitySystemComponent()),
		                 TEXT("%s owner have a invalid AbilitySystemComponent"), *GetName()))
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

void APEPlayerController::OnAbilityInputReleased(UInputAction* Action) const
{
	if (!IsValid(GetPawn()))
	{
		CONTROLLER_BASE_VLOG(this, Warning, TEXT(" %s called with invalid Pawn"), *FString(__func__));
		return;
	}

	const uint32 InputID = AbilityActionBindings.FindRef(Action).InputID;

	CONTROLLER_BASE_VLOG(this, Display, TEXT(" %s called with Input ID Value %u"), *FString(__func__), InputID);

	// Check if controller owner is valid and owns a ability system component
	if (const APECharacter* ControllerOwner = GetPawn<APECharacter>();
		ensureAlwaysMsgf(IsValid(ControllerOwner->GetAbilitySystemComponent()),
		                 TEXT("%s owner have a invalid AbilitySystemComponent"), *GetName()))
	{
		// Send the input released event to the ability system component with the found input ID
		ControllerOwner->GetAbilitySystemComponent()->AbilityLocalInputReleased(InputID);
	}
}

void APEPlayerController::SetVoiceChatEnabled(const FInputActionValue& Value) const
{
	CONTROLLER_BASE_VLOG(this, Display, TEXT(" %s called with Input Action Value %s (magnitude %f)"),
	                     *FString(__func__),
	                     *Value.ToString(), Value.GetMagnitude());

	// [EOS] Call the static function responsible for activating/deactivating the voice chat
	UPEEOSLibrary::MuteEOSSessionVoiceChatUser(NetPlayerIndex, !Value.Get<bool>());
}

#pragma region Default Movement Functions
void APEPlayerController::ChangeCameraAxis(const FInputActionValue& Value)
{
	if (!IsValid(GetPawnOrSpectator()))
	{
		CONTROLLER_AXIS_VLOG(this, Warning, TEXT(" %s called with invalid Pawn"), *FString(__func__));
		return;
	}

	CONTROLLER_AXIS_VLOG(this, Display, TEXT(" %s called with Input Action Value %s (magnitude %f)"),
	                     *FString(__func__), *Value.ToString(), Value.GetMagnitude());

	AddYawInput(-1.f * Value[1] * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	AddPitchInput(Value[0] * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void APEPlayerController::Move(const FInputActionValue& Value) const
{
	if (!IsValid(GetPawnOrSpectator()))
	{
		CONTROLLER_AXIS_VLOG(this, Warning, TEXT(" %s called with invalid Pawn"), *FString(__func__));
		return;
	}

	CONTROLLER_AXIS_VLOG(this, Display, TEXT(" %s called with Input Action Value %s (magnitude %f)"),
	                     *FString(__func__), *Value.ToString(), Value.GetMagnitude());

	if (Value.GetMagnitude() != 0.0f && !IsMoveInputIgnored())
	{
		const FRotator YawRotation(0, GetControlRotation().Yaw, 0);

		const FVector DirectionX = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector DirectionY = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		GetPawnOrSpectator()->AddMovementInput(DirectionX, Value[1]);
		GetPawnOrSpectator()->AddMovementInput(DirectionY, Value[0]);

		UE_VLOG_LOCATION(GetPawn(), LogController_Axis, Log, GetPawn()->GetActorLocation(), 25.f, FColor::Green,
		                 TEXT("%s"), *GetPawn()->GetName());
	}
}

void APEPlayerController::Jump(const FInputActionValue& Value) const
{
	if (!IsValid(GetPawn()))
	{
		CONTROLLER_AXIS_VLOG(this, Warning, TEXT(" %s called with invalid Pawn"), *FString(__func__));
		return;
	}

	CONTROLLER_BASE_VLOG(this, Display, TEXT(" %s called with Input Action Value %s (magnitude %f)"),
	                     *FString(__func__), *Value.ToString(), Value.GetMagnitude());

	if (APECharacter* ControllerOwner = GetPawn<APECharacter>())
	{
		Value.Get<bool>()
			? ControllerOwner->Jump()
			: ControllerOwner->StopJumping();
	}
}
#pragma endregion Default Movement Functions
