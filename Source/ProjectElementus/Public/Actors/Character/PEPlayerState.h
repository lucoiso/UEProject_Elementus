// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "PEPlayerState.generated.h"

class UAttributeSet;
class UGameplayEffect;
class UPEAbilitySystemComponent;
class UDataTable;
struct FGameplayTag;
/**
 *
 */
DECLARE_LOG_CATEGORY_EXTERN(LogPlayerState, Display, NoLogging);

#define PLAYERSTATE_VLOG(Actor, Verbosity, Format, ...) \
{ \
	UE_LOG(LogPlayerState, Verbosity, Format, ##__VA_ARGS__); \
	UE_VLOG(Actor, LogPlayerState, Verbosity, Format, ##__VA_ARGS__); \
}
/**
 *
 */
UCLASS(NotBlueprintable, NotPlaceable, Category = "Custom Classes | Player")
class PROJECTELEMENTUS_API APEPlayerState final : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	explicit APEPlayerState(const FObjectInitializer& ObjectInitializer);

private:
	void DeathStateChanged_Callback(const FGameplayTag CallbackTag, int32 NewCount) const;
	void StunStateChanged_Callback(const FGameplayTag CallbackTag, int32 NewCount) const;

protected:
	virtual void BeginPlay() override;

	/* Returns a casted version of Player Controller using PEPlayerController class */
	UFUNCTION(BlueprintPure, Category = "Custom GAS | Components",
		meta = (DisplayName = "Get Casted Player Controller: APEPlayerController"))
	class APEPlayerController* GetPEPlayerController() const;

	/* Player associated Ability System Component */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Custom GAS | Components",
		meta = (AllowPrivateAccess = "true"))
	UPEAbilitySystemComponent* AbilitySystemComponent;

public:
	/* Returns associated Ability System Component */
	UFUNCTION(BlueprintPure, Category = "Custom GAS | Components")
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
};
