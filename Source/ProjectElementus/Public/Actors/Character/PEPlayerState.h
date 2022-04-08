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

	class APEPlayerController* GetPEPlayerController() const;

	TWeakObjectPtr<UGASAbilitySystemComponent> AbilitySystemComponent;
	TWeakObjectPtr<class UGASAttributeSet> Attributes;

	TWeakObjectPtr<UDataTable> LevelingData;
	TWeakObjectPtr<UDataTable> AttributesData;

public:
	UFUNCTION(BlueprintCallable, Category = "Custom GAS | Components")
		virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable, Category = "Custom GAS | Components")
		UAttributeSet* GetAttributeSetBase() const;

	UFUNCTION(BlueprintCallable, Category = "Custom GAS | Components")
		TArray<UAttributeSet*> GetAttributeSetArray() const;

	UFUNCTION(BlueprintCallable, Category = "Custom GAS | Attributes")
		const float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Custom GAS | Attributes")
		const float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Custom GAS | Attributes")
		const float GetStamina() const;

	UFUNCTION(BlueprintCallable, Category = "Custom GAS | Attributes")
		const float GetMaxStamina() const;

	UFUNCTION(BlueprintCallable, Category = "Custom GAS | Attributes")
		const float GetMana() const;

	UFUNCTION(BlueprintCallable, Category = "Custom GAS | Attributes")
		const float GetMaxMana() const;

	UFUNCTION(BlueprintCallable, Category = "Custom GAS | Attributes")
		const float GetAttackRate() const;

	UFUNCTION(BlueprintCallable, Category = "Custom GAS | Attributes")
		const float GetDefenseRate() const;

	UFUNCTION(BlueprintCallable, Category = "Custom GAS | Attributes")
		const float GetSpeedRate() const;

	UFUNCTION(BlueprintCallable, Category = "Custom GAS | Attributes")
		const float GetJumpRate() const;

	UFUNCTION(BlueprintCallable, Category = "Custom GAS | Attributes")
		const float GetExperience() const;

	UFUNCTION(BlueprintCallable, Category = "Custom GAS | Attributes")
		const float GetGold() const;

	UFUNCTION(BlueprintCallable, Category = "Custom GAS | Attributes")
		const float GetLevelingRequirementExp() const;

protected:
	void SetupCharacterLevel(const uint32 NewLevel);

private:
	float NextLevelRequirement = 0.f;
	void ExperienceChanged_Callback(const FOnAttributeChangeData& Data);

	TSubclassOf<UGameplayEffect> DeathEffect;
};
