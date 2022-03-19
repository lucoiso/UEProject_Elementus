// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Actors/Character/PECharacter.h"
#include "Actors/Character/PEAIController.h"
#include "Actors/Character/PEPlayerState.h"

#include "Abilities/GameplayAbility.h"

#include "GAS/System/GASAbilitySystemComponent.h"
#include "GAS/System/GASAttributeSet.h"

APECharacter::APECharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AIControllerClass = APEAIController::StaticClass();
}

void APECharacter::BindASCInput()
{
	if (!bInputBound && AbilitySystemComponent.IsValid() && IsValid(InputComponent))
	{
		const FGameplayAbilityInputBinds Binds(
			"Confirm", "Cancel", InputIDEnumerationClass->GetFName().ToString(),
			InputIDEnumerationClass->GetValueByName("Confirm", EGetByNameFlags::CheckAuthoredName),
			InputIDEnumerationClass->GetValueByName("Cancel", EGetByNameFlags::CheckAuthoredName));

		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, Binds);

		bInputBound = true;
	}
}

void APECharacter::PossessedBy(AController* InputController)
{
	Super::PossessedBy(InputController);

	InitializeAttributes(false);
}

void APECharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitializeAttributes(true);
}

void APECharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->RefreshAbilityActorInfo();

		const FGameplayTag DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
		AbilitySystemComponent->SetTagMapCount(DeadTag, 0);
	}
}

void APECharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (!AbilitySystemComponent.IsValid())
	{
		return;
	}

	const FGameplayTag DoubleJumpTag = FGameplayTag::RequestGameplayTag(FName("GameplayAbility.Default.DoubleJump"));

	if (AbilitySystemComponent->HasMatchingGameplayTag(DoubleJumpTag))
	{
		const FGameplayAbilitySpec& AbilitySpec =
			*AbilitySystemComponent->FindAbilitySpecFromInputID(
				InputIDEnumerationClass->GetValueByName("Jump", EGetByNameFlags::CheckAuthoredName));

#if __cplusplus > 201402L // Detect if compiler version is > c++14
		if constexpr (&AbilitySpec != nullptr)
#else
		if (&AbilitySpec != nullptr)
#endif
		{
			if (AbilitySpec.Handle.IsValid())
			{
				AbilitySystemComponent->CancelAbilityHandle(AbilitySpec.Handle);
			}
		}
	}
}