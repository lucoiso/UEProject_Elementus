// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "ElementusInventoryData.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "PECharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterDeath);

class UPEEquipment;
class UPEAbilitySystemComponent;
class UGameplayAbility;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class UElementusInventoryComponent;
/**
 *
 */
UCLASS(config = Game, Category = "Project Elementus | Classes")
class PROJECTELEMENTUS_API APECharacter final : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

protected:
	virtual void PossessedBy(AController* InController) override;
	virtual void OnRep_PlayerState() override;
	virtual void OnRep_Controller() override;

private:
	TWeakObjectPtr<UPEAbilitySystemComponent> AbilitySystemComponent;

public:
	explicit APECharacter(const FObjectInitializer& ObjectInitializer);

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

	/* Initialize the specified Ability System Component with the given owner actor in this character (AvatarActor) */
	void InitializeAbilitySystemComponent(UAbilitySystemComponent* InABSC, AActor* InOwnerActor);

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Project Elementus | Properties")
	TObjectPtr<UElementusInventoryComponent> InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Project Elementus | Properties")
	TMap<FGameplayTag, FElementusItemInfo> EquipmentMap;

	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	void EquipItem(const FElementusItemInfo& InItem);

	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	void UnnequipItem(FElementusItemInfo& InItem);

protected:
	float DefaultWalkSpeed, DefaultCrouchSpeed, DefaultJumpVelocity;

	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	/* Init a death state with this character */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	void PerformDeath();

	UFUNCTION(BlueprintImplementableEvent, Category = "Project Elementus | Functions | Callbacks")
	void AbilityActivated(UGameplayAbility* Ability);

	UFUNCTION(BlueprintImplementableEvent, Category = "Project Elementus | Functions | Callbacks")
	void AbilityCommited(UGameplayAbility* Ability);

	UFUNCTION(BlueprintImplementableEvent, Category = "Project Elementus | Functions | Callbacks")
	void AbilityEnded(UGameplayAbility* Ability);

	UFUNCTION(BlueprintNativeEvent, Category = "Project Elementus | Functions | Callbacks")
	void AbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& TagContainer);

	UPROPERTY(BlueprintAssignable, Category = "Project Elementus | Delegates")
	FOnCharacterDeath OnCharacterDeath;

private:
	UFUNCTION(Server, Reliable)
	void Server_PerformDeath();

	UFUNCTION(Server, Reliable)
	void Server_SpawnInventoryPackage();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_DeathSetup();

	virtual void Landed(const FHitResult& Hit) override;

#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Project Elementus | Debug", meta = (AllowPrivateAccess = "true"))
	bool bDebugAbilities = true;
#endif
};
