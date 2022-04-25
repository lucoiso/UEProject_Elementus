// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "PECharacter.generated.h"

class UGameplayAbility;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
struct FGameplayTag;
/**
 *
 */
UCLASS(config = Game, Category = "Custom Classes | Player")
class PROJECTELEMENTUS_API APECharacter final : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

protected:
	virtual void PossessedBy(AController* InputController) override;
	virtual void OnRep_PlayerState() override;
	virtual void OnRep_Controller() override;

private:
	void InitializeABSC(const bool bOnRep);
	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	bool bIsFrameworkReady = false;

public:
	explicit APECharacter(const FObjectInitializer& ObjectInitializer);

	FORCEINLINE virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("Character", GetFName());
	}

	/** Returns CameraBoom sub object **/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const
	{
		return CameraBoom;
	}

	/** Returns FollowCamera sub object **/
	FORCEINLINE UCameraComponent* GetFollowCamera() const
	{
		return FollowCamera;
	}

	/** Returns FollowCamera Forward Vector **/
	FVector GetCameraForwardVector() const;

	/** Returns FollowCamera Location at World **/
	FVector GetCameraComponentLocation() const;

	/** Returns FollowCamera Location at World **/
	float GetCameraTargetArmLength() const;

	/* Returns character default walk speed */
	UFUNCTION(BlueprintPure, Category = "Custom GAS | Attributes")
	float GetDefaultWalkSpeed() const;

	/* Returns character default crouch speed */
	UFUNCTION(BlueprintPure, Category = "Custom GAS | Attributes")
	float GetDefaultCrouchSpeed() const;

	/* Returns character default jump velocity */
	UFUNCTION(BlueprintPure, Category = "Custom GAS | Attributes")
	float GetDefaultJumpVelocity() const;

	/* Returns character associated Ability System Component */
	UFUNCTION(BlueprintPure, Category = "Custom GAS | Components")
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/* Enumeration class used to bind ability InputIDs */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "InputID Enumeration Class"),
		Category = "Custom GAS | Data")
	UEnum* InputIDEnumerationClass;

protected:
	float DefaultWalkSpeed, DefaultCrouchSpeed, DefaultJumpVelocity;

	/* Array of given abilities */
	UPROPERTY(BlueprintReadOnly, Category = "Custom GAS | Abilities")
	TArray<TSubclassOf<UGameplayAbility>> CharacterAbilities;

	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	/* Give a new Ability to the Player -  bAutoAdjustInput will ignore InputId and select Skill_1, Skill_2 or Skill_3 based on current owned abilities */
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Custom GAS | Abilities")
	void GiveAbility(TSubclassOf<UGameplayAbility> Ability, const FName InputId,
	                 const bool bTryRemoveExistingAbilityWithInput, const bool bTryRemoveExistingAbilityWithClass);

	/* Will remove the ability associated to the InputAction */
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Custom GAS | Abilities")
	void RemoveAbility(TSubclassOf<UGameplayAbility> Ability);

	/* Init a death state with this character */
	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Custom GAS | Behaviors")
	void PerformDeath();
	bool PerformDeath_Validate();

	UFUNCTION(BlueprintImplementableEvent, Category = "Custom GAS | Abilities")
	void AbilityActivated(UGameplayAbility* Ability);

	UFUNCTION(BlueprintImplementableEvent, Category = "Custom GAS | Abilities")
	void AbilityCommited(UGameplayAbility* Ability);

	UFUNCTION(BlueprintImplementableEvent, Category = "Custom GAS | Abilities")
	void AbilityEnded(UGameplayAbility* Ability);

	UFUNCTION(BlueprintImplementableEvent, Category = "Custom GAS | Abilities")
	void AbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& Reason);
	virtual void AbilityFailed_Implementation(const UGameplayAbility* Ability, const FGameplayTagContainer& Reason);

private:
	virtual void Landed(const FHitResult& Hit) override;
};
