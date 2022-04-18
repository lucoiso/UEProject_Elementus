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
class UGASAbilitySystemComponent;
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
class PROJECTELEMENTUS_API APEPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	APEPlayerState(const FObjectInitializer& ObjectInitializer);

private:
	void DeathStateChanged_Callback(const FGameplayTag CallbackTag, int32 NewCount) const;
	void StunStateChanged_Callback(const FGameplayTag CallbackTag, int32 NewCount) const;

	void HealthChanged_Callback(const struct FOnAttributeChangeData& Data) const;
	void StaminaChanged_Callback(const FOnAttributeChangeData& Data) const;
	void ManaChanged_Callback(const FOnAttributeChangeData& Data) const;
	void SpeedRateChanged_Callback(const FOnAttributeChangeData& Data) const;
	void JumpRateChanged_Callback(const FOnAttributeChangeData& Data) const;

protected:
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/* Returns a casted version of Player Controller using PEPlayerController class */
	UFUNCTION(BlueprintPure, Category = "Custom GAS | Components", meta = (DisplayName = "Get Casted Player Controller: APEPlayerController"))
		class APEPlayerController* GetPEPlayerController() const;

	TWeakObjectPtr<UGASAbilitySystemComponent> AbilitySystemComponent;
	TWeakObjectPtr<class UGASAttributeSet> Attributes;

	/* This DataTable contains informations about attribute values that will be passed when the level is changed */
	TWeakObjectPtr<UDataTable> LevelingData;

	/* This DataTable contains informations about attribute values that will be passed on initialization */
	TWeakObjectPtr<UDataTable> AttributesData;

public:
	/* Returns associated Ability System Component */
	UFUNCTION(BlueprintPure, Category = "Custom GAS | Components")
		virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/* Returns main Attribute Set associated to Ability System Component */
	UFUNCTION(BlueprintPure, Category = "Custom GAS | Components")
		UAttributeSet* GetAttributeSetBase() const;

	/* Returns a array of secondary mutable attributes from Ability System Component */
	UFUNCTION(BlueprintPure, Category = "Custom GAS | Components")
		TArray<UAttributeSet*> GetAttributeSetArray() const;

	/* Returns current Health property value from associated AttributeSet */
	UFUNCTION(BlueprintPure, Category = "Custom GAS | Attributes")
		const float GetHealth() const;

	/* Returns current MaxHealth property value from associated AttributeSet */
	UFUNCTION(BlueprintPure, Category = "Custom GAS | Attributes")
		const float GetMaxHealth() const;

	/* Returns current Stamina property value from associated AttributeSet */
	UFUNCTION(BlueprintPure, Category = "Custom GAS | Attributes")
		const float GetStamina() const;

	/* Returns current MaxStamina property value from associated AttributeSet */
	UFUNCTION(BlueprintPure, Category = "Custom GAS | Attributes")
		const float GetMaxStamina() const;

	/* Returns current Mana property value from associated AttributeSet */
	UFUNCTION(BlueprintPure, Category = "Custom GAS | Attributes")
		const float GetMana() const;

	/* Returns current MaxMana property value from associated AttributeSet */
	UFUNCTION(BlueprintPure, Category = "Custom GAS | Attributes")
		const float GetMaxMana() const;

	/* Returns current AttackRate property value from associated AttributeSet */
	UFUNCTION(BlueprintPure, Category = "Custom GAS | Attributes")
		const float GetAttackRate() const;

	/* Returns current DefenseRate property value from associated AttributeSet */
	UFUNCTION(BlueprintPure, Category = "Custom GAS | Attributes")
		const float GetDefenseRate() const;

	/* Returns current SpeedRate property value from associated AttributeSet */
	UFUNCTION(BlueprintPure, Category = "Custom GAS | Attributes")
		const float GetSpeedRate() const;

	/* Returns current JumpRate property value from associated AttributeSet */
	UFUNCTION(BlueprintPure, Category = "Custom GAS | Attributes")
		const float GetJumpRate() const;

	/* Returns current Experience property value from associated AttributeSet */
	UFUNCTION(BlueprintPure, Category = "Custom GAS | Attributes")
		const float GetExperience() const;

	/* Returns current Gold property value from associated AttributeSet */
	UFUNCTION(BlueprintPure, Category = "Custom GAS | Attributes")
		const float GetGold() const;

	/* Returns current value of NextLevelRequirement which is automatically set when the player level changes */
	UFUNCTION(BlueprintPure, Category = "Custom GAS | Attributes")
		const float GetLevelingRequirementExp() const;

protected:
	/* Change character level and updates its attributes based on: TWeakObjectPtr<UDataTable> LevelingData */
	UFUNCTION(BlueprintCallable, Category = "Custom GAS | Management")
		void SetupCharacterLevel(const int32 NewLevel);

private:
	float NextLevelRequirement = 0.f;
	void ExperienceChanged_Callback(const FOnAttributeChangeData& Data);

	TSubclassOf<UGameplayEffect> DeathEffect;
};
