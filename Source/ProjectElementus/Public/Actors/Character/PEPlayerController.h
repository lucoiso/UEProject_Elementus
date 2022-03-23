// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "InputTriggers.h"
#include "AbilityInputBinding.h"
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

struct FAbilityInputData
{
public:
	uint32 OnPressedHandle = 0;
	uint32 OnReleasedHandle = 0;
	uint32 InputID = 0;
};

/**
 *
 */
UCLASS(NotBlueprintable, NotPlaceable, Category = "Custom Classes | Player")
class PROJECTELEMENTUS_API APEPlayerController : public APlayerController, public IAbilityInputBinding
{
	GENERATED_BODY()

public:
	APEPlayerController(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(Client, Reliable)
		virtual void SetupAbilityInput(UInputAction* Action, const int32 InputID) override;
	virtual void SetupAbilityInput_Implementation(UInputAction* Action, const int32 InputID);

	UFUNCTION(Client, Reliable)
		virtual void RemoveAbilityInputBinding(const UInputAction* Action) const override;
	virtual void RemoveAbilityInputBinding_Implementation(const UInputAction* Action) const;

protected:
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Custom Properties | Management")
		TSubclassOf<UUserWidget> HUDClass;

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnRep_PlayerState() override;

private:
	TWeakObjectPtr<UUserWidget> HUDHandle;
	TMap<UInputAction*, FAbilityInputData> AbilityActionBindings;

	UFUNCTION()
		void ChangeCameraAxis(const FInputActionValue& Value);
	UFUNCTION()
		void Move(const FInputActionValue& Value);
	UFUNCTION()
		void Jump(const FInputActionValue& Value);

	void OnAbilityInputPressed(UInputAction* Action);
	void OnAbilityInputReleased(UInputAction* Action);
};
