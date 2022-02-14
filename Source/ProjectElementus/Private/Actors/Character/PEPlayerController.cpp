// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Actors/Character/PEPlayerController.h"
#include "Actors/Character/PECharacterBase.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "AbilitySystemComponent.h"
#include "Components/GameFrameworkComponentManager.h"

DEFINE_LOG_CATEGORY(LogController_Base);
DEFINE_LOG_CATEGORY(LogController_Axis);

APEPlayerController::APEPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void APEPlayerController::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void APEPlayerController::BeginPlay()
{
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(
		this, UGameFrameworkComponentManager::NAME_GameActorReady);

	Super::BeginPlay();
}

void APEPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);

	Super::EndPlay(EndPlayReason);
}

void APEPlayerController::SetupAbilityInput_Implementation(UInputAction* Action, const int32 InputID)
{
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);

	if (IsValid(EnhancedInputComponent))
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

void APEPlayerController::RemoveAbilityInputBinding_Implementation(const UInputAction* Action) const
{
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);

	if (IsValid(EnhancedInputComponent))
	{
		EnhancedInputComponent->RemoveBindingByHandle(AbilityActionBindings.FindRef(Action).OnPressedHandle);
		EnhancedInputComponent->RemoveBindingByHandle(AbilityActionBindings.FindRef(Action).OnReleasedHandle);
	}
}

void APEPlayerController::OnAbilityInputPressed(UInputAction* Action)
{
	const int32 InputID = AbilityActionBindings.FindRef(Action).InputID;

	CONTROLLER_BASE_VLOG(this, Warning, TEXT(" %s called with Input ID Value %u"),
	                     TEXT(__FUNCTION__), InputID);

	const APECharacterBase* ControllerOwner = Cast<APECharacterBase>(GetCharacter());

	if (IsValid(ControllerOwner) && IsValid(ControllerOwner->GetAbilitySystemComponent()))
	{
		ControllerOwner->GetAbilitySystemComponent()->AbilityLocalInputPressed(InputID);
	}
}

void APEPlayerController::OnAbilityInputReleased(UInputAction* Action)
{
	const int32 InputID = AbilityActionBindings.FindRef(Action).InputID;

	CONTROLLER_BASE_VLOG(this, Warning, TEXT(" %s called with Input ID Value %u"),
	                     TEXT(__FUNCTION__), InputID);

	const APECharacterBase* ControllerOwner = Cast<APECharacterBase>(GetCharacter());

	if (IsValid(ControllerOwner) && IsValid(ControllerOwner->GetAbilitySystemComponent()))
	{
		ControllerOwner->GetAbilitySystemComponent()->AbilityLocalInputReleased(InputID);
	}
}

void APEPlayerController::ChangeCameraAxis(const FInputActionValue& Value)
{
	CONTROLLER_AXIS_VLOG(this, Warning, TEXT(" %s called with Input Action Value %s (magnitude %f)"),
	                     TEXT(__FUNCTION__),
	                     *Value.ToString(), Value.GetMagnitude());

	AddYawInput(-1.f * Value[1] * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	AddPitchInput(Value[0] * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void APEPlayerController::Move(const FInputActionValue& Value)
{
	CONTROLLER_AXIS_VLOG(this, Warning, TEXT(" %s called with Input Action Value %s (magnitude %f)"),
	                     TEXT(__FUNCTION__),
	                     *Value.ToString(), Value.GetMagnitude());

	if (Value.GetMagnitude() != 0.0f && !IsMoveInputIgnored())
	{
		const FRotator YawRotation(0, GetControlRotation().Yaw, 0);

		const FVector DirectionX = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector DirectionY = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		GetPawn()->AddMovementInput(DirectionX, Value[1]);
		GetPawn()->AddMovementInput(DirectionY, Value[0]);

		UE_VLOG_LOCATION(GetPawn(), LogController_Axis, Log, GetPawn()->GetActorLocation(), 25.f, FColor::Green,
		                 TEXT("%s"), *GetPawn()->GetName());
	}
}

void APEPlayerController::Jump(const FInputActionValue& Value)
{
	CONTROLLER_BASE_VLOG(this, Warning, TEXT(" %s called with Input Action Value %s (magnitude %f)"),
	                     TEXT(__FUNCTION__),
	                     *Value.ToString(), Value.GetMagnitude());

	APECharacterBase* ControllerOwner = Cast<APECharacterBase>(GetCharacter());

	if (IsValid(ControllerOwner) && ControllerOwner->CanJump() && !IsMoveInputIgnored())
	{
		ControllerOwner->Jump();
	}
}
