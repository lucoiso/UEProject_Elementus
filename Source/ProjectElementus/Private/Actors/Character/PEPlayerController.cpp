// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Actors/Character/PEPlayerController.h"
#include "Actors/Character/PECharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedPlayerInput.h"
#include "InputAction.h"

#include "AbilitySystemComponent.h"
#include "Actors/Character/PEHUD.h"

constexpr float BaseTurnRate = 45.f;
constexpr float BaseLookUpRate = 45.f;

DEFINE_LOG_CATEGORY(LogController_Base);
DEFINE_LOG_CATEGORY(LogController_Axis);

APEPlayerController::APEPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void APEPlayerController::RemoveHUD_Implementation()
{
	if (APEHUD* HUD_Temp = GetHUD<APEHUD>(); ensureMsgf(IsValid(HUD_Temp), TEXT("%s have a invalid HUD"), *GetName()))
	{
		HUD_Temp->HideHUD();
	}
}

// Double "_Implementation" because this function is a RPC call version of a virtual function from IAbilityBinding interface
void APEPlayerController::SetupAbilityInputBinding_Implementation_Implementation(
	UInputAction* Action, const int32 InputID)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent); ensureMsgf(
		IsValid(EnhancedInputComponent), TEXT("%s have a invalid EnhancedInputComponent"), *GetName()))
	{
		FAbilityInputData AbilityBinding
		{
			AbilityBinding.OnPressedHandle =
			EnhancedInputComponent->BindAction(Action, ETriggerEvent::Started, this,
			                                   &APEPlayerController::OnAbilityInputPressed, Action).GetHandle(),
			AbilityBinding.OnReleasedHandle =
			EnhancedInputComponent->BindAction(Action, ETriggerEvent::Completed, this,
			                                   &APEPlayerController::OnAbilityInputReleased, Action).GetHandle(),
			AbilityBinding.InputID = InputID
		};

		AbilityActionBindings.Add(Action, AbilityBinding);
	}
}

// Double "_Implementation" because this function is a RPC call version of a virtual function from IAbilityBinding interface
void APEPlayerController::RemoveAbilityInputBinding_Implementation_Implementation(const UInputAction* Action) const
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent); ensureMsgf(
		IsValid(EnhancedInputComponent), TEXT("%s have a invalid EnhancedInputComponent"), *GetName()))
	{
		EnhancedInputComponent->RemoveBindingByHandle(AbilityActionBindings.FindRef(Action).OnPressedHandle);
		EnhancedInputComponent->RemoveBindingByHandle(AbilityActionBindings.FindRef(Action).OnReleasedHandle);
	}
}

void APEPlayerController::OnAbilityInputPressed(UInputAction* Action) const
{
	if (!IsValid(GetPawn()))
	{
		CONTROLLER_BASE_VLOG(this, Warning, TEXT(" %s called with invalid Pawn"), *FString(__func__));

		return;
	}

	const int32 InputID = AbilityActionBindings.FindRef(Action).InputID;

	CONTROLLER_BASE_VLOG(this, Display, TEXT(" %s called with Input ID Value %u"),
	                     *FString(__func__), InputID);

	if (const APECharacter* ControllerOwner = GetPawn<APECharacter>(); ensureMsgf(
		IsValid(ControllerOwner) && IsValid(ControllerOwner->GetAbilitySystemComponent()),
		TEXT("%s have a invalid ControllerOwner"), *GetName()))
	{
		ControllerOwner->GetAbilitySystemComponent()->AbilityLocalInputPressed(InputID);

		if (InputID == ControllerOwner->InputIDEnumerationClass->GetValueByName(
			"Confirm", EGetByNameFlags::CheckAuthoredName))
		{
			ControllerOwner->GetAbilitySystemComponent()->LocalInputConfirm();
		}

		else if (InputID == ControllerOwner->InputIDEnumerationClass->GetValueByName(
			"Cancel", EGetByNameFlags::CheckAuthoredName))
		{
			ControllerOwner->GetAbilitySystemComponent()->LocalInputCancel();
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

	const int32 InputID = AbilityActionBindings.FindRef(Action).InputID;

	CONTROLLER_BASE_VLOG(this, Display, TEXT(" %s called with Input ID Value %u"),
	                     *FString(__func__), InputID);

	if (const APECharacter* ControllerOwner = GetPawn<APECharacter>(); ensureMsgf(
		IsValid(ControllerOwner) && IsValid(ControllerOwner->GetAbilitySystemComponent()),
		TEXT("%s have a invalid ControllerOwner"), *GetName()))
	{
		ControllerOwner->GetAbilitySystemComponent()->AbilityLocalInputReleased(InputID);
	}
}

void APEPlayerController::ChangeCameraAxis(const FInputActionValue& Value)
{
	if (!IsValid(GetPawnOrSpectator()))
	{
		CONTROLLER_AXIS_VLOG(this, Warning, TEXT(" %s called with invalid Pawn"), *FString(__func__));

		return;
	}

	CONTROLLER_AXIS_VLOG(this, Display, TEXT(" %s called with Input Action Value %s (magnitude %f)"),
	                     *FString(__func__),
	                     *Value.ToString(), Value.GetMagnitude());

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
	                     *FString(__func__),
	                     *Value.ToString(), Value.GetMagnitude());

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
	                     *FString(__func__),
	                     *Value.ToString(), Value.GetMagnitude());

	if (APECharacter* ControllerOwner = GetPawn<APECharacter>(); ensureMsgf(
		IsValid(ControllerOwner), TEXT("%s have a invalid ControllerOwner"), *GetName()))
	{
		if (ControllerOwner->CanJump() && !IsMoveInputIgnored())
		{
			ControllerOwner->Jump();
		}
	}
}
