// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Management/Functions/PEPlayerLibrary.h"
#include "EnhancedInputSubsystems.h"

FPlayerInputBindingHandle UPEPlayerLibrary::BindDynamicInput(APlayerController* Controller, UInputAction* Action,
                                                             UObject* Object, const FName UFunctionName,
                                                             const ETriggerEvent TriggerEvent)
{
	if (IsValid(Controller))
	{
		if (UEnhancedInputComponent* EnhancedInputComponent =
				Cast<UEnhancedInputComponent>(Controller->InputComponent.Get());
			ensureAlwaysMsgf(IsValid(EnhancedInputComponent), TEXT("%s have a invalid EnhancedInputComponent"),
			                 *Controller->GetName()))
		{
			return FPlayerInputBindingHandle
			{
				Controller,
				static_cast<int32>(EnhancedInputComponent->BindAction(Action, TriggerEvent, Object, UFunctionName).
				                                           GetHandle())
			};
		}
	}

	return FPlayerInputBindingHandle{};
}

void UPEPlayerLibrary::RemoveDynamicInput(const FPlayerInputBindingHandle BindingHandle)
{
	if (IsValid(BindingHandle.PlayerController))
	{
		if (UEnhancedInputComponent* EnhancedInputComponent =
				Cast<UEnhancedInputComponent>(BindingHandle.PlayerController->InputComponent.Get());
			ensureAlwaysMsgf(IsValid(EnhancedInputComponent), TEXT("%s have a invalid EnhancedInputComponent"),
			                 *BindingHandle.PlayerController->GetName()))
		{
			EnhancedInputComponent->RemoveBindingByHandle(BindingHandle.InputBindingHandle);
		}
	}
}

void UPEPlayerLibrary::AddDynamicMapping(APlayerController* Controller,
                                         UInputMappingContext* InputMapping, const int32 Priority)
{
	if (IsValid(Controller))
	{
		if (const ULocalPlayer* LocalPlayer = Controller->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
				LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				Subsystem->AddMappingContext(InputMapping, Priority);
			}
		}
	}
}

void UPEPlayerLibrary::RemoveDynamicMapping(APlayerController* Controller, UInputMappingContext* InputMapping)
{
	if (IsValid(Controller))
	{
		if (const ULocalPlayer* LocalPlayer = Controller->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
				LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				Subsystem->RemoveMappingContext(InputMapping);
			}
		}
	}
}

bool UPEPlayerLibrary::CheckIfPlayerContainsDynamicInput(APlayerController* Controller,
                                                         TArray<FPlayerInputBindingHandle> BindingArray,
                                                         FPlayerInputBindingHandle& BindingHandle)
{
	if (IsValid(Controller) && !BindingArray.IsEmpty())
	{
		for (const auto& [PlayerController, InputBindingHandle] : BindingArray)
		{
			if (PlayerController == Controller)
			{
				BindingHandle = {PlayerController, InputBindingHandle};
				return true;
			}
		}
	}

	return false;
}
