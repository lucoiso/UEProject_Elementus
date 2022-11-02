// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEModularFeatures_ExtraActions

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilityInputBinding.h"
#include "GameFeatureAction_WorldActionBase.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/Controller.h"

namespace ModularFeaturesHelper
{
	static UAbilitySystemComponent* GetAbilitySystemComponentByActor(AActor* InActor)
	{
		const IAbilitySystemInterface* const InterfaceOwner = Cast<IAbilitySystemInterface>(InActor);
		return InterfaceOwner != nullptr ? InterfaceOwner->GetAbilitySystemComponent() : InActor->FindComponentByClass<UAbilitySystemComponent>();
	}

	static IAbilityInputBinding* GetAbilityInputBindingInterface(AActor* InActor, const EControllerOwner& InOwner)
	{
		if (!IsValid(InActor))
		{
			return nullptr;
		}

		if (APawn* const TargetPawn = Cast<APawn>(InActor))
		{
			switch (InOwner)
			{
			case EControllerOwner::Pawn: return Cast<IAbilityInputBinding>(TargetPawn);

			case EControllerOwner::Controller: return Cast<IAbilityInputBinding>(TargetPawn->GetController());

			default: return nullptr;
			}
		}

		return nullptr;
	}

	static UEnhancedInputComponent* GetEnhancedInputComponentInPawn(APawn* InPawn, const EControllerOwner& InOwner)
	{
		if (!IsValid(InPawn))
		{
			return nullptr;
		}

		switch (InOwner)
		{
		case EControllerOwner::Pawn: return Cast<UEnhancedInputComponent>(InPawn->InputComponent.Get());

		case EControllerOwner::Controller: return Cast<UEnhancedInputComponent>(InPawn->GetController()->InputComponent.Get());

		default: break;
		}

		return nullptr;
	}

	static AActor* GetPawnInputOwner(APawn* InPawn, const EControllerOwner& InOwner)
	{
		if (!IsValid(InPawn))
		{
			return nullptr;
		}

		switch (InOwner)
		{
		case EControllerOwner::Pawn: return Cast<AActor>(InPawn);

		case EControllerOwner::Controller: return Cast<AActor>(InPawn->GetController());

		default: break;
		}

		return nullptr;
	}
}
