// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "InputTriggers.h"
#include "AbilityInputBinding.h"
#include "AbilitySystemInterface.h"
#include "PEPlayerController.generated.h"

/**
 *
 */
DECLARE_LOG_CATEGORY_EXTERN(LogController_Base, Display, Verbose);

DECLARE_LOG_CATEGORY_EXTERN(LogController_Axis, Display, NoLogging);

#define CONTROLLER_BASE_VLOG(Actor, Verbosity, Format, ...) \
{ \
	UE_LOG(LogController_Base, Verbosity, Format, ##__VA_ARGS__); \
	UE_VLOG(Actor, LogController_Base, Verbosity, Format, ##__VA_ARGS__); \
}

#define CONTROLLER_AXIS_VLOG(Actor, Verbosity, Format, ...) \
{ \
	UE_LOG(LogController_Axis, Verbosity, Format, ##__VA_ARGS__); \
	UE_VLOG(Actor, LogController_Axis, Verbosity, Format, ##__VA_ARGS__); \
}

/**
 *
 */
UCLASS(NotBlueprintable, NotPlaceable, Category = "Project Elementus | Classes")
class PROJECTELEMENTUS_API APEPlayerController final : public APlayerController, public IAbilityInputBinding
{
	GENERATED_BODY()

public:
	explicit APEPlayerController(const FObjectInitializer& ObjectInitializer);

#pragma region IAbilityInputBinding
	/* This function came from IAbilityInputBinding interface,
	 * provided by GameFeatures_ExtraActions plugin to manage ability bindings */
	UFUNCTION(Client, Reliable)
	virtual void SetupAbilityInputBinding_Implementation(UInputAction* Action, const int32 InputID) override;

	/* This function came from IAbilityInputBinding interface,
	 * provided by GameFeatures_ExtraActions plugin to manage ability bindings */
	UFUNCTION(Client, Reliable)
	virtual void RemoveAbilityInputBinding_Implementation(const UInputAction* Action) const override;
#pragma endregion IAbilityInputBinding

	/* Setup the spectating state on both client and server */
	UFUNCTION(NetMulticast, Unreliable)
	void SetupControllerSpectator();

	/* Will respawn the character if the player is in spectating state */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	void InitializeRespawn(const float InSeconds);

protected:
	/* Perform the respawn task on server */
	UFUNCTION(Server, Reliable)
	void RespawnAndPossess();

private:
	struct FAbilityInputData
	{
		uint32 OnPressedHandle = 0;
		uint32 OnReleasedHandle = 0;
		uint32 InputID = 0;
	};

	TWeakObjectPtr<UEnum> InputEnumHandle;
	TMap<UInputAction*, FAbilityInputData> AbilityActionBindings;

	UFUNCTION()
	void ChangeCameraAxis(const FInputActionValue& Value);
	UFUNCTION()
	void Move(const FInputActionValue& Value) const;
	UFUNCTION()
	void Jump(const FInputActionValue& Value) const;

	void OnAbilityInputPressed(UInputAction* Action) const;
	void OnAbilityInputReleased(UInputAction* Action) const;

	UFUNCTION()
	void SetVoiceChatEnabled(const FInputActionValue& Value) const;
};
