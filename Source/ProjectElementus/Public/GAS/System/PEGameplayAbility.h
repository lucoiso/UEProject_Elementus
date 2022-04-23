// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GAS/System/PEAbilityData.h"
#include "GAS/System/PETrace.h"
#include "PEGameplayAbility.generated.h"

class AGameplayAbilityTargetActor;
class APEProjectileActor;
/**
 *
 */
UCLASS(Abstract, NotPlaceable, Blueprintable, HideDropdown, Category = "Custom GAS | Components")
class PROJECTELEMENTUS_API UPEGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UPEGameplayAbility(const FObjectInitializer& ObjectInitializer);

	FORCEINLINE virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("Ability", GetFName());
	}

	/* This will determine the line trace distance to perform */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Custom GAS | Defaults")
		float AbilityMaxRange;

	/* If set to true, ability will ignore Cost GE application */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Custom GAS | Defaults")
		bool bIgnoreCost;

	/* If set to true, ability will ignore Cooldown GE application */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Custom GAS | Defaults")
		bool bIgnoreCooldown;

protected:
	/* Mix with bEndAbilityAfterActiveTime to end ability with a pre-determined time */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom GAS | Defaults")
		float AbilityActiveTime;

	/* Will finish ability when the AbilityActiveTime is complete */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom GAS | Defaults")
		bool bEndAbilityAfterActiveTime;

	/* Auto activate this ability when granted by Ability System Component */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom GAS | Defaults")
		bool bAutoActivateOnGrant;

	/* Gameplay Effects that will be applied to self via ApplyAbilityEffectsToSelf function */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom GAS | Defaults")
		TArray<FGameplayEffectGroupedData> SelfAbilityEffects;

	/* Gameplay Effects that will be applied to target via ApplyAbilityEffectsToTarget function */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom GAS | Defaults")
		TArray<FGameplayEffectGroupedData> TargetAbilityEffects;

	/* Single Montage to play with ActivateWaitMontageTask function - You can mix this with ActivateWaitGameplayEventTask function */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom GAS | Defaults")
		UAnimMontage* AbilityAnimation;

private:
	virtual void
		OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override final;

	virtual void PreActivate(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
		const FGameplayEventData* TriggerEventData = nullptr) override final;

	/*
	* This canceling task will only be used to cancel ability when the Cancel Input is pressed
	* Declared as private to avoid multiple uses of it
	*/
	/* This task will wait for canceling input */
	void ActivateWaitCancelInputTask();

	/* Default callback for ActivateWaitCancelInputTask */
	UFUNCTION(Category = "Custom GAS | Helpers | Callbacks")
		void WaitCancelInput_Callback();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override final;

	virtual bool CommitAbilityCooldown(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const bool ForceCooldown,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) override final;

	virtual bool CommitAbilityCost(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) override final;

	virtual void CommitExecute(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override final;

	/*
	* These 'Activate Task' and 'Callback' functions are intended to act as helper functions
	* They will call the defaults tasks from original GAS source
	* This is a way to avoid the need to include the same headers in every ability class
	* This is also a way to avoid the need to copy the same code in every ability class
	*/

	/* Wait Confirm input and call WaitConfirmInput_Callback function */
	UFUNCTION(BlueprintCallable, Category = "Custom GAS | Helpers | Delegates")
		void ActivateWaitConfirmInputTask();

	/* Wait a Gameplay Tag to be added to owner and call WaitAddedTag_Callback function */
	UFUNCTION(BlueprintCallable, Category = "Custom GAS | Helpers | Delegates")
		void ActivateWaitAddedTagTask(const FGameplayTag Tag);

	/* Wait a Gameplay Tag to be removed from owner and call WaitRemovedTag_Callback function */
	UFUNCTION(BlueprintCallable, Category = "Custom GAS | Helpers | Delegates")
		void ActivateWaitRemovedTagTask(const FGameplayTag Tag);

	/* Perform a animation montage and call WaitMontage_Callback function */
	UFUNCTION(BlueprintCallable, Category = "Custom GAS | Helpers | Delegates")
		void ActivateWaitMontageTask(const FName MontageSection = NAME_None, const float Rate = 1.f,
			const bool bRandomSection = false, const bool bStopsWhenAbilityEnds = true);

	/* Performs targeting and call WaitTargetData_Callback function */
	UFUNCTION(BlueprintCallable, Category = "Custom GAS | Helpers | Delegates")
		void ActivateWaitTargetDataTask(const TEnumAsByte<EGameplayTargetingConfirmation::Type> TargetingConfirmation,
			const TSubclassOf<AGameplayAbilityTargetActor_Trace> TargetActorClass,
			struct FTargetActorSpawnParams TargetParameters);

	/* Start a task to wait for a Gameplay Event and call WaitGameplayEvent_Callback function */
	UFUNCTION(BlueprintCallable, Category = "Custom GAS | Helpers | Delegates")
		void ActivateWaitGameplayEventTask(const FGameplayTag EventTag);

	/* Spawn a actor and call SpawnActor_Callback function */
	UFUNCTION(BlueprintCallable, Category = "Custom GAS | Helpers | Delegates")
		void ActivateSpawnActorTask(const FGameplayAbilityTargetDataHandle TargetDataHandle,
			TSubclassOf<AActor> ActorClass);

	/* Default callback for ActivateWaitMontageTask function */
	UFUNCTION(BlueprintNativeEvent, Category = "Custom GAS | Helpers | Callbacks")
		void WaitMontage_Callback();

	virtual void WaitMontage_Callback_Implementation()
	{
	}; // Override this function on children classes.

	/* Default callback for ActivateWaitConfirmInputTask function */
	UFUNCTION(BlueprintNativeEvent, Category = "Custom GAS | Helpers | Callbacks")
		void WaitConfirmInput_Callback();

	virtual void WaitConfirmInput_Callback_Implementation()
	{
	}; // Override this function on children classes.

	/* Default callback for ActivateWaitAddedTagTask function */
	UFUNCTION(BlueprintNativeEvent, Category = "Custom GAS | Helpers | Callbacks")
		void WaitAddedTag_Callback();

	virtual void WaitAddedTag_Callback_Implementation()
	{
	}; // Override this function on children classes.

	/* Default callback for ActivateWaitRemovedTagTask function */
	UFUNCTION(BlueprintNativeEvent, Category = "Custom GAS | Helpers | Callbacks")
		void WaitRemovedTag_Callback();

	virtual void WaitRemovedTag_Callback_Implementation()
	{
	}; // Override this function on children classes.

	/* Default callback for ActivateWaitAttributeChangeTask function */
	UFUNCTION(BlueprintNativeEvent, Category = "Custom GAS | Helpers | Callbacks")
		void WaitAttributeChange_Callback();

	virtual void WaitAttributeChange_Callback_Implementation()
	{
	}; // Override this function on children classes.

	/* Default callback for ActivateWaitGameplayEventTask function */
	UFUNCTION(BlueprintNativeEvent, Category = "Custom GAS | Helpers | Callbacks")
		void WaitGameplayEvent_Callback(FGameplayEventData Payload);

	virtual void WaitGameplayEvent_Callback_Implementation(FGameplayEventData Payload)
	{
	}; // Override this function on children classes.

	/* Default callback for ActivateWaitTargetDataTask function */
	UFUNCTION(BlueprintNativeEvent, Category = "Custom GAS | Helpers | Callbacks")
		void WaitTargetData_Callback(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	virtual void WaitTargetData_Callback_Implementation(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
	{
	}; // Override this function on children classes.

	/* Default callback for ActivateSpawnActorTask function */
	UFUNCTION(BlueprintNativeEvent, Category = "Custom GAS | Helpers | Callbacks")
		void SpawnActor_Callback(AActor* SpawnedActor);

	virtual void SpawnActor_Callback_Implementation(AActor* SpawnedActor)
	{
	}; // Override this function on children classes.

	/* Activate a Gameplay Cue with passed parameters */
	UFUNCTION(BlueprintCallable, Category = "Custom GAS | Management")
		void ActivateGameplayCues(const FGameplayTag GameplayCueTag, FGameplayCueParameters Parameters,
			UAbilitySystemComponent* SourceAbilitySystem);

	/* Apply SelfAbilityEffects to self */
	UFUNCTION(BlueprintCallable, DisplayName = "ApplyAbilityEffectsToSelf", Category = "Custom GAS | Management")
		void BP_ApplyAbilityEffectsToSelf();

	void ApplyAbilityEffectsToSelf(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo);

	/* Remove SelfAbilityEffects from self */
	UFUNCTION(BlueprintCallable, DisplayName = "RemoveAbilityEffectsFromSelf", Category = "Custom GAS | Management")
		void BP_RemoveAbilityEffectsFromSelf();

	void RemoveAbilityEffectsFromSelf(const FGameplayAbilityActorInfo* ActorInfo);

	/* Apply TargetAbilityEffects to target */
	UFUNCTION(BlueprintCallable, DisplayName = "ApplyAbilityEffectsToTarget", Category = "Custom GAS | Management")
		void BP_ApplyAbilityEffectsToTarget(const FGameplayAbilityTargetDataHandle TargetDataHandle);

	void ApplyAbilityEffectsToTarget(const FGameplayAbilityTargetDataHandle TargetDataHandle,
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo);

	/* Spawn and fire a projectile with TargetAbilityEffects effects applied */
	UFUNCTION(BlueprintCallable, DisplayName = "SpawnProjectileWithTargetEffects", Category = "Custom GAS | Management")
		void BP_SpawnProjectileWithTargetEffects(
			const TSubclassOf<APEProjectileActor> ProjectileClass,
			const FTransform ProjectileTransform,
			const FVector ProjectileFireDirection);

	void SpawnProjectileWithTargetEffects(
		TSubclassOf<APEProjectileActor> ProjectileClass,
		const FTransform ProjectileTransform,
		const FVector ProjectileFireDirection,
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo);

	/* Default callback for SpawnProjectileWithTargetEffects function */
	UFUNCTION(BlueprintNativeEvent, Category = "Custom GAS | Helpers | Callbacks")
		void SpawnProjectile_Callback(APEProjectileActor* SpawnedProjectile);

	virtual void SpawnProjectile_Callback_Implementation(APEProjectileActor* SpawnedProjectile)
	{
	}; // Override this function on children classes.

	/* Remove the cooldown effect */
	UFUNCTION(BlueprintCallable, Category = "Custom GAS | Management")
		void RemoveCooldownEffect(UAbilitySystemComponent* SourceAbilitySystem) const;

	/* Shared Timer Handle that is actually used with bEndAbilityAfterActiveTime */
	FTimerHandle CancelationTimerHandle;
};
