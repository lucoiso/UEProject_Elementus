// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Management/Functions/PEPlayerLibrary.h"
#include <EnhancedInputSubsystems.h>
#include <EnhancedInputComponent.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(PEPlayerLibrary)

FPlayerInputBindingHandle UPEPlayerLibrary::BindDynamicInput(APlayerController* Controller, UInputAction* Action, UObject* Object, const FName UFunctionName, const ETriggerEvent TriggerEvent)
{
	if (!IsValid(Controller) || !IsValid(Action) || !IsValid(Object))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s - Invalid Controller, Action or Object."), *FString(__func__));
		return FPlayerInputBindingHandle{};
	}
	
	if (UEnhancedInputComponent* const EnhancedInputComponent = Cast<UEnhancedInputComponent>(Controller->InputComponent.Get());
		ensureAlwaysMsgf(IsValid(EnhancedInputComponent), TEXT("%s have a invalid EnhancedInputComponent"), *Controller->GetName()))
	{
		return FPlayerInputBindingHandle{ Controller, static_cast<int32>(EnhancedInputComponent->BindAction(Action, TriggerEvent, Object, UFunctionName).GetHandle()) };
	}

	return FPlayerInputBindingHandle{};
}

void UPEPlayerLibrary::RemoveDynamicInput(const FPlayerInputBindingHandle BindingHandle)
{
	if (!IsValid(BindingHandle.PlayerController))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s - Invalid controller."), *FString(__func__));
		return;
	}

	if (UEnhancedInputComponent* const EnhancedInputComponent = Cast<UEnhancedInputComponent>(BindingHandle.PlayerController->InputComponent.Get());
		ensureAlwaysMsgf(IsValid(EnhancedInputComponent), TEXT("%s have a invalid EnhancedInputComponent"), *BindingHandle.PlayerController->GetName()))
	{
		EnhancedInputComponent->RemoveBindingByHandle(BindingHandle.InputBindingHandle);
	}
}

void UPEPlayerLibrary::AddDynamicMapping(APlayerController* Controller, UInputMappingContext* InputMapping, const int32 Priority)
{
	if (!IsValid(Controller) || !InputMapping)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s - Invalid controller or input mapping."), *FString(__func__));
		return;
	}
	
	if (const ULocalPlayer* const LocalPlayer = Controller->GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* const Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			Subsystem->AddMappingContext(InputMapping, Priority);
		}
	}
}

void UPEPlayerLibrary::RemoveDynamicMapping(APlayerController* Controller, UInputMappingContext* InputMapping)
{
	if (!IsValid(Controller) || !InputMapping)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s - Invalid controller or input mapping."), *FString(__func__));
		return;
	}
	
	if (const ULocalPlayer* const LocalPlayer = Controller->GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* const Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			Subsystem->RemoveMappingContext(InputMapping);
		}
	}
}

bool UPEPlayerLibrary::CheckIfPlayerContainsDynamicInput(APlayerController* Controller, TArray<FPlayerInputBindingHandle> BindingArray, FPlayerInputBindingHandle& BindingHandle)
{
	if (BindingArray.IsEmpty() || !IsValid(Controller))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s - Invalid controller or empty binding array."), *FString(__func__));
		return false;
	}
	
	for (const auto& [PlayerController, InputBindingHandle] : BindingArray)
	{
		if (PlayerController == Controller)
		{
			BindingHandle = { PlayerController, InputBindingHandle };
			return true;
		}
	}

	return false;
}

FVector UPEPlayerLibrary::GetSocketLocationInChildMeshes(const USkeletalMeshComponent* ParentMesh, const FName SocketName)
{
	if (SocketName.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("%s - Invalid socket name"), *FString(__func__));
		return FVector::Zero();
	}
	
	const TArray<TObjectPtr<USceneComponent>>& AttachChildrenArr = ParentMesh->GetAttachChildren();
	if (AttachChildrenArr.IsEmpty())
	{
		return FVector::Zero();
	}

	for (const TObjectPtr<USceneComponent>& Iterator : AttachChildrenArr)
	{
		if (const USkeletalMeshComponent* const Mesh = Cast<USkeletalMeshComponent>(Iterator); IsValid(Mesh) && Mesh->DoesSocketExist(SocketName))
		{
			return Mesh->GetSocketLocation(SocketName);
		}
	}

	return FVector::Zero();
}
