// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "PECharacterBase.generated.h"

class UGASAbilitySystemComponent;
class UGameplayAbility;
class UAttributeSet;
class APEPlayerState;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
struct FGameplayTag;
struct FOnAttributeChangeData;
/**
 *
 */
UCLASS(config = Game, Abstract, Category = "Custom Classes | Player")
class PROJECTELEMENTUS_API APECharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	bool bIsFrameworkReady = false;

protected:
	TWeakObjectPtr<UGASAbilitySystemComponent> AbilitySystemComponent;
	TWeakObjectPtr<class UGASAttributeSet> Attributes;

public:
	APECharacterBase(const FObjectInitializer& ObjectInitializer);

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
	const FVector GetCameraForwardVector() const;

	/** Returns FollowCamera Location at World **/
	const FVector GetCameraComponentLocation() const;

	/** Returns FollowCamera Location at World **/
	const float GetCameraTargetArmLength() const;

	/* Returns character default walk speed */
	UFUNCTION(BlueprintPure, Category = "Custom GAS | Attributes")
		const float GetDefaultWalkSpeed() const;

	/* Returns character default crouch speed */
	UFUNCTION(BlueprintPure, Category = "Custom GAS | Attributes")
		const float GetDefaultCrouchSpeed() const;

	/* Returns character default jump velocity */
	UFUNCTION(BlueprintPure, Category = "Custom GAS | Attributes")
		const float GetDefaultJumpVelocity() const;

	/* Returns character associated Ability System Component */
	UFUNCTION(BlueprintPure, Category = "Custom GAS | Components")
		virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/* Returns main Attribute Set associated to Ability System Component */
	UFUNCTION(BlueprintPure, Category = "Custom GAS | Components")
		UAttributeSet* GetAttributeSetBase() const;

	/* Returns a array of secondary mutable attributes from Ability System Component */
	UFUNCTION(BlueprintPure, Category = "Custom GAS | Components")
		TArray<UAttributeSet*> GetAttributeSetArray() const;

	/* Returns a casted version of Player State using PEPlayerState class */
	UFUNCTION(BlueprintPure, Category = "Custom GAS | Components", meta = (DisplayName = "Get Casted Player State: APEPlayerState"))
		APEPlayerState* GetPEPlayerState() const;

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

	void InitializeAttributes(const bool bOnRep);

public:
	/* Give a new Ability to the Player -  bAutoAdjustInput will ignore InputId and select Skill_1, Skill_2 or Skill_3 based on current owned abilities */
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Custom GAS | Abilities")
		void GiveAbility(TSubclassOf<UGameplayAbility> Ability, const FName InputId,
			const bool bTryRemoveExistingAbilityWithInput, const bool bTryRemoveExistingAbilityWithClass);
	virtual void GiveAbility_Implementation(TSubclassOf<UGameplayAbility> Ability, const FName InputId,
		const bool bTryRemoveExistingAbilityWithInput, const bool bTryRemoveExistingAbilityWithClass);

	/* Will remove the ability associated to the InputAction */
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Custom GAS | Abilities")
		void RemoveAbility(TSubclassOf<UGameplayAbility> Ability);
	virtual void RemoveAbility_Implementation(TSubclassOf<UGameplayAbility> Ability);

	/* Init a death state with this character */
	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Custom GAS | Behaviors")
		void PerformDeath();
	virtual void PerformDeath_Implementation();
	bool PerformDeath_Validate();
};
