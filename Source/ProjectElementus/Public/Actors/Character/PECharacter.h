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
UCLASS(config = Game, Category = "Project Elementus | Classes")
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
	UFUNCTION(BlueprintPure, Category = "Project Elementus | Functions")
	float GetDefaultWalkSpeed() const;

	/* Returns character default crouch speed */
	UFUNCTION(BlueprintPure, Category = "Project Elementus | Functions")
	float GetDefaultCrouchSpeed() const;

	/* Returns character default jump velocity */
	UFUNCTION(BlueprintPure, Category = "Project Elementus | Functions")
	float GetDefaultJumpVelocity() const;

	/* Returns character associated Ability System Component */
	UFUNCTION(BlueprintPure, Category = "Project Elementus | Functions")
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	float DefaultWalkSpeed, DefaultCrouchSpeed, DefaultJumpVelocity;

	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;

public:
	/* Init a death state with this character */
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "Project Elementus | Functions")
	void PerformDeath();

	UFUNCTION(BlueprintImplementableEvent, Category = "Project Elementus | Functions | Callbacks")
	void AbilityActivated(UGameplayAbility* Ability);

	UFUNCTION(BlueprintImplementableEvent, Category = "Project Elementus | Functions | Callbacks")
	void AbilityCommited(UGameplayAbility* Ability);

	UFUNCTION(BlueprintImplementableEvent, Category = "Project Elementus | Functions | Callbacks")
	void AbilityEnded(UGameplayAbility* Ability);

	UFUNCTION(BlueprintNativeEvent, Category = "Project Elementus | Functions | Callbacks")
	void AbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& Reason);

private:
	virtual void Landed(const FHitResult& Hit) override;
};
