// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "GAS/System/PEEffectData.h"
#include "Abilities/GameplayAbility.h"
#include "PEGameplayAbility.generated.h"

class AGameplayAbilityTargetActor_Trace;
class AGameplayAbilityTargetActor;
class APEProjectileActor;
struct FPETargetActorSpawnParams;
/**
 *
 */
UCLASS(Abstract, NotPlaceable, Blueprintable, HideDropdown, Category = "Project Elementus | Classes")
class PROJECTELEMENTUS_API UPEGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	explicit UPEGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/* This will determine the line trace distance to perform */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Project Elementus | Properties")
	float AbilityMaxRange;

	/* If set to true, ability will ignore Cost GE application */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Project Elementus | Properties")
	bool bIgnoreCost;

	/* If set to true, ability will ignore Cooldown GE application */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Project Elementus | Properties")
	bool bIgnoreCooldown;

protected:
	/* If true, ability will wait for Cancel Input to cancel this ability */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties")
	bool bWaitCancel;

	/* Set by Caller parameters that will be applied to Ability Cost - For Duration: 0.0 for instantaneous effects; -1.0 for infinite duration. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties")
	TMap<FGameplayTag, float> AbilityCostSetByCallerData;

	/* Set by Caller parameters that will be applied to Ability Cooldown */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties")
	TMap<FGameplayTag, float> AbilityCooldownSetByCallerData;

	/* Tags used to check cooldown with SetByCaller data */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties")
	FGameplayTagContainer SetByCallerCooldownTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties")
	TObjectPtr<USoundBase> AbilitySoundFX;
	
	/* Mix with bEndAbilityAfterActiveTime to end ability with a pre-determined time */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties")
	float AbilityActiveTime;

	/* Will finish ability when the AbilityActiveTime is complete */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties")
	bool bEndAbilityAfterActiveTime;

	/* Auto activate this ability when granted by Ability System Component */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties")
	bool bAutoActivateOnGrant;

	/* Gameplay Effects that will be applied to self via ApplyAbilityEffectsToSelf function */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties")
	TArray<FGameplayEffectGroupedData> SelfAbilityEffects;

	/* Gameplay Effects that will be applied to target via ApplyAbilityEffectsToTarget function */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties")
	TArray<FGameplayEffectGroupedData> TargetAbilityEffects;

	/* Single Montage to play with ActivateWaitMontageTask function - You can mix this with ActivateWaitGameplayEventTask function */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties")
	TObjectPtr<UAnimMontage> AbilityAnimation;

private:
	/*
	* This canceling task will only be used to cancel ability when the Cancel Input is pressed
	* Declared as private to avoid multiple uses of it
	*/
	/* This task will wait for canceling input */
	void ActivateWaitCancelInputTask();

	/* Default callback for ActivateWaitCancelInputTask */
	UFUNCTION(Category = "Project Elementus | Functions | Callbacks")
	void WaitCancelInput_Callback();

protected:
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override final;

	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override final;

	virtual void PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData = nullptr) override final;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override final;

	virtual bool CommitAbilityCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const bool ForceCooldown, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) override final;

	virtual bool CommitAbilityCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) override final;

	virtual void CommitExecute(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override final;

	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;
	
	virtual bool CheckCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual const FGameplayTagContainer* GetCooldownTags() const override;

	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;
	
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	
	/* Apply Set by Caller data map to the given Gameplay Effect Spec */
	void ApplySetByCallerParamsToEffectSpec(const TMap<FGameplayTag, float>& SetByCallerData, const TSharedPtr<FGameplayEffectSpec>& EffectSpec) const;

	/*
	* These 'Activate Task' and 'Callback' functions are intended to act as helper functions
	* They will call the defaults tasks from original GAS source
	* This is a way to avoid the need to include the same headers in every ability class
	* This is also a way to avoid the need to copy the same code in every ability class
	*/

	/* Wait Confirm input and call WaitConfirmInput_Callback function */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions | Delegates")
	void ActivateWaitConfirmInputTask();

	/* Wait a Gameplay Tag to be added to owner and call WaitAddedTag_Callback function */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions | Delegates")
	void ActivateWaitAddedTagTask(const FGameplayTag Tag);

	/* Wait a Gameplay Tag to be removed from owner and call WaitRemovedTag_Callback function */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions | Delegates")
	void ActivateWaitRemovedTagTask(const FGameplayTag Tag);

	/* Perform a animation montage and call WaitMontage_Callback function */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions | Delegates")
	void ActivateWaitMontageTask(const FName MontageSection = NAME_None, const float Rate = 1.f, const bool bRandomSection = false, const bool bStopsWhenAbilityEnds = true);

	/* Performs targeting and call WaitTargetData_Callback function */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions | Delegates")
	void ActivateWaitTargetDataTask(const TEnumAsByte<EGameplayTargetingConfirmation::Type> TargetingConfirmation, const TSubclassOf<AGameplayAbilityTargetActor_Trace> TargetActorClass, FPETargetActorSpawnParams TargetParameters);

	/* Start a task to wait for a Gameplay Event and call WaitGameplayEvent_Callback function */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions | Delegates")
	void ActivateWaitGameplayEventTask(const FGameplayTag EventTag);

	/* Spawn a actor and call SpawnActor_Callback function */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions | Delegates")
	void ActivateSpawnActorTask(const FGameplayAbilityTargetDataHandle TargetDataHandle, TSubclassOf<AActor> ActorClass);

	/* Default callback for ActivateWaitMontageTask function */
	UFUNCTION(BlueprintNativeEvent, Category = "Project Elementus | Functions | Callbacks")
	void WaitMontage_Callback();

	virtual void WaitMontage_Callback_Implementation()
	{
	} // Override this function on children classes.

	/* Default callback for ActivateWaitConfirmInputTask function */
	UFUNCTION(BlueprintNativeEvent, Category = "Project Elementus | Functions | Callbacks")
	void WaitConfirmInput_Callback();

	virtual void WaitConfirmInput_Callback_Implementation()
	{
	} // Override this function on children classes.

	/* Default callback for ActivateWaitAddedTagTask function */
	UFUNCTION(BlueprintNativeEvent, Category = "Project Elementus | Functions | Callbacks")
	void WaitAddedTag_Callback();

	virtual void WaitAddedTag_Callback_Implementation()
	{
	} // Override this function on children classes.

	/* Default callback for ActivateWaitRemovedTagTask function */
	UFUNCTION(BlueprintNativeEvent, Category = "Project Elementus | Functions | Callbacks")
	void WaitRemovedTag_Callback();

	virtual void WaitRemovedTag_Callback_Implementation()
	{
	} // Override this function on children classes.

	/* Default callback for ActivateWaitAttributeChangeTask function */
	UFUNCTION(BlueprintNativeEvent, Category = "Project Elementus | Functions | Callbacks")
	void WaitAttributeChange_Callback();

	virtual void WaitAttributeChange_Callback_Implementation()
	{
	} // Override this function on children classes.

	/* Default callback for ActivateWaitGameplayEventTask function */
	UFUNCTION(BlueprintNativeEvent, Category = "Project Elementus | Functions | Callbacks")
	void WaitGameplayEvent_Callback(FGameplayEventData Payload);

	virtual void WaitGameplayEvent_Callback_Implementation(FGameplayEventData Payload)
	{
	} // Override this function on children classes.

	/* Default callback for ActivateWaitTargetDataTask function */
	UFUNCTION(BlueprintNativeEvent, Category = "Project Elementus | Functions | Callbacks")
	void WaitTargetData_Callback(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	virtual void WaitTargetData_Callback_Implementation(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
	{
	} // Override this function on children classes.

	/* Default callback for ActivateSpawnActorTask function */
	UFUNCTION(BlueprintNativeEvent, Category = "Project Elementus | Functions | Callbacks")
	void SpawnActor_Callback(AActor* SpawnedActor);

	virtual void SpawnActor_Callback_Implementation(AActor* SpawnedActor)
	{
	} // Override this function on children classes.

	/* Activate a Gameplay Cue with passed parameters */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	void ActivateGameplayCues(const FGameplayTag GameplayCueTag, FGameplayCueParameters Parameters, UAbilitySystemComponent* SourceAbilitySystem = nullptr);

	/* Apply SelfAbilityEffects to self */
	UFUNCTION(BlueprintCallable, DisplayName = "ApplyAbilityEffectsToSelf", Category = "Project Elementus | Functions")
	void BP_ApplyAbilityEffectsToSelf();

	void ApplyAbilityEffectsToSelf(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo);

	/* Remove SelfAbilityEffects from self */
	UFUNCTION(BlueprintCallable, DisplayName = "RemoveAbilityEffectsFromSelf", Category = "Project Elementus | Functions")
	void BP_RemoveAbilityEffectsFromSelf();

	void RemoveAbilityEffectsFromSelf(const FGameplayAbilityActorInfo* ActorInfo);

	/* Apply TargetAbilityEffects to target */
	UFUNCTION(BlueprintCallable, DisplayName = "ApplyAbilityEffectsToTarget", Category = "Project Elementus | Functions")
	void BP_ApplyAbilityEffectsToTarget(const FGameplayAbilityTargetDataHandle TargetDataHandle);

	void ApplyAbilityEffectsToTarget(const FGameplayAbilityTargetDataHandle TargetDataHandle, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo);

	/* Spawn and fire a projectile with TargetAbilityEffects effects applied */
	UFUNCTION(BlueprintCallable, DisplayName = "SpawnProjectileWithTargetEffects", Category = "Project Elementus | Functions")
	void BP_SpawnProjectileWithTargetEffects(const TSubclassOf<APEProjectileActor> ProjectileClass, const FTransform ProjectileTransform, const FVector ProjectileFireDirection);

	void SpawnProjectileWithTargetEffects(TSubclassOf<APEProjectileActor> ProjectileClass, const FTransform ProjectileTransform, const FVector ProjectileFireDirection, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo);

	/* Default callback for SpawnProjectileWithTargetEffects function */
	UFUNCTION(BlueprintNativeEvent, Category = "Project Elementus | Functions | Callbacks")
	void SpawnProjectile_Callback(APEProjectileActor* SpawnedProjectile);

	virtual void SpawnProjectile_Callback_Implementation(APEProjectileActor* SpawnedProjectile)
	{
	} // Override this function on children classes.

	/* Remove the cooldown effect */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	void RemoveCooldownEffect(UAbilitySystemComponent* SourceAbilitySystem) const;

	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	void PlayAbilitySoundAttached(USceneComponent* InComponent, const FName SocketToAttach = NAME_None, const FVector& InLocation = FVector::ZeroVector, const float InVolumeMultiplier = 1.f);

	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	void PlayAbilitySoundAtLocation(const UObject* WorldContext, const FVector& InLocation = FVector::ZeroVector, const float InVolumeMultiplier = 1.f);
	
	/* Shared Timer Handle that is actually used with bEndAbilityAfterActiveTime */
	FTimerHandle CancelationTimerHandle;

	/* Extra tags to manage tasks conditions */
	FGameplayTagContainer AbilityExtraTags;
};
