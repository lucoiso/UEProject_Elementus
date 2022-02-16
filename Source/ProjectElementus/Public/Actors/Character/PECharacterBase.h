// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "PECharacterBase.generated.h"

class UGameplayEffect;
class UGASAbilitySystemComponent;
class UGameplayAbility;
class UAttributeSet;
class UDataTable;
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

	TSubclassOf<UGameplayEffect> DeathEffect;

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
	FVector GetCameraForwardVector() const;

	/** Returns FollowCamera Location at World **/
	FVector GetCameraComponentLocation() const;

	/** Returns FollowCamera Location at World **/
	float GetCameraTargetArmLength() const;

	UFUNCTION(BlueprintCallable, Category = "Custom GAS | Attributes")
	float GetDefaultWalkSpeed() const;

	UFUNCTION(BlueprintCallable, Category = "Custom GAS | Attributes")
	float GetDefaultCrouchSpeed() const;

	UFUNCTION(BlueprintCallable, Category = "Custom GAS | Attributes")
	float GetDefaultJumpVelocity() const;

	UFUNCTION(BlueprintCallable, Category = "Custom GAS | Components")
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable, Category = "Custom GAS | Components")
	UAttributeSet* GetAttributeSetBase() const;

	UFUNCTION(BlueprintCallable, Category = "Custom GAS | Components")
	TArray<UAttributeSet*> GetAttributeSetArray() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "InputID Enumeration Class"),
		Category = "Custom GAS | Data")
	UEnum* InputIDEnumerationClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom GAS | Data")
	UDataTable* LevelingData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom GAS | Data")
	TSoftObjectPtr<UDataTable> AttributesData;

protected:
	float DefaultWalkSpeed, DefaultCrouchSpeed, DefaultJumpVelocity;

	UPROPERTY(BlueprintReadOnly, Category = "Custom GAS | Abilities")
	TArray<TSubclassOf<UGameplayAbility>> CharacterAbilities;

	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void InitializeAttributes(const bool bOnRep);

	bool bAttributesInitialized = false;

private:
	UFUNCTION(NetMulticast, Reliable, WithValidation, Category = "Custom GAS | Data")
	void SetupCharacterLevel(const uint32 NewLevel);
	virtual void SetupCharacterLevel_Implementation(const uint32 NewLevel);
	virtual bool SetupCharacterLevel_Validate(const uint32 NewLevel);

	void ExperienceChanged_Callback(const FOnAttributeChangeData& Data);

	float NextLevelRequirement = 0;

public:
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Custom GAS | Abilities")
	void GiveAbility(TSubclassOf<UGameplayAbility> Ability);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Custom GAS | Abilities")
	void RemoveAbility(TSubclassOf<UGameplayAbility> Ability);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Custom GAS | Behaviors")
	void Die();

	void FinishDying();
};
