// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Actors/Character/PEPlayerState.h"
#include "Actors/Character/PECharacterBase.h"

#include "GAS/System/GASAbilitySystemComponent.h"
#include "GAS/System/GASAttributeSet.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/GameFrameworkComponentManager.h"

DEFINE_LOG_CATEGORY(LogPlayerState);

APEPlayerState::APEPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilitySystemComponent = CreateDefaultSubobject<UGASAbilitySystemComponent>(TEXT("Ability System Component"));
	Attributes = CreateDefaultSubobject<UGASAttributeSet>(TEXT("Attributes Set Base"));

	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	NetUpdateFrequency = 75.f;
}


void APEPlayerState::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void APEPlayerState::BeginPlay()
{
	PLAYERSTATE_VLOG(this, Warning, TEXT(" %s called."), TEXT(__FUNCTION__));

	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(
		this, UGameFrameworkComponentManager::NAME_GameActorReady);

	Super::BeginPlay();

	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attributes->GetSpeedRateAttribute()).
		                        AddUObject(
			                        this, &APEPlayerState::SpeedRateChanged_Callback);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attributes->GetJumpRateAttribute()).
		                        AddUObject(
			                        this, &APEPlayerState::JumpRateChanged_Callback);

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attributes->GetHealthAttribute()).
		                        AddUObject(
			                        this, &APEPlayerState::HealthChanged_Callback);

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attributes->GetStaminaAttribute()).
		                        AddUObject(
			                        this, &APEPlayerState::StaminaChanged_Callback);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attributes->GetManaAttribute()).
		                        AddUObject(
			                        this, &APEPlayerState::ManaChanged_Callback);

		AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("State.Dead")),
		                                                 EGameplayTagEventType::NewOrRemoved).AddUObject(
			this, &APEPlayerState::DeathStateChanged_Callback);

		AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("State.Stunned")),
		                                                 EGameplayTagEventType::NewOrRemoved).AddUObject(
			this, &APEPlayerState::StunStateChanged_Callback);
	}
}

void APEPlayerState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);

	Super::EndPlay(EndPlayReason);
}

void APEPlayerState::DeathStateChanged_Callback(const FGameplayTag CallbackTag, const int32 NewCount) const
{
	PLAYERSTATE_VLOG(this, Warning, TEXT(" %s called with %s Callback Tag and NewCount equal to %d"),
	                 TEXT(__FUNCTION__),
	                 *CallbackTag.ToString(), NewCount);

	if (NewCount != 0)
	{
		APECharacterBase* Player = Cast<APECharacterBase>(GetPawn());

		if (IsValid(Player))
		{
			Player->Die();
		}
	}
}

void APEPlayerState::StunStateChanged_Callback(const FGameplayTag CallbackTag, const int32 NewCount) const
{
	PLAYERSTATE_VLOG(this, Warning, TEXT(" %s called with %s Callback Tag and NewCount equal to %d"),
	                 TEXT(__FUNCTION__),
	                 *CallbackTag.ToString(), NewCount);

	const APECharacterBase* Player = Cast<APECharacterBase>(GetPawn());

	if (IsValid(Player) && IsValid(Player->GetController()))
	{
		Player->GetController()->SetIgnoreMoveInput(NewCount != 0);
	}
}

void APEPlayerState::HealthChanged_Callback(const FOnAttributeChangeData& Data) const
{
	PLAYERSTATE_VLOG(this, Warning, TEXT(" %s called with %f value"), TEXT(__FUNCTION__), Data.NewValue);

	APECharacterBase* Player = Cast<APECharacterBase>(GetPawn());

	if (IsValid(Player))
	{
		if (Data.NewValue <= 0.f)
		{
			Player->Die();
		}
	}
}

void APEPlayerState::StaminaChanged_Callback(const FOnAttributeChangeData& Data) const
{
	PLAYERSTATE_VLOG(this, Warning, TEXT(" %s called with %f value"), TEXT(__FUNCTION__), Data.NewValue);

	const APECharacterBase* Player = Cast<APECharacterBase>(GetPawn());

	if (IsValid(Player))
	{
	}
}

void APEPlayerState::ManaChanged_Callback(const FOnAttributeChangeData& Data) const
{
	PLAYERSTATE_VLOG(this, Warning, TEXT(" %s called with %f value"), TEXT(__FUNCTION__), Data.NewValue);

	const APECharacterBase* Player = Cast<APECharacterBase>(GetPawn());

	if (IsValid(Player))
	{
	}
}

void APEPlayerState::SpeedRateChanged_Callback(const FOnAttributeChangeData& Data) const
{
	PLAYERSTATE_VLOG(this, Warning, TEXT(" %s called with %f value"), TEXT(__FUNCTION__), Data.NewValue);

	const APECharacterBase* Player = Cast<APECharacterBase>(GetPawn());

	if (IsValid(Player))
	{
		UCharacterMovementComponent* MovComp = Player->GetCharacterMovement();

		MovComp->MaxWalkSpeed = Data.NewValue * Player->GetDefaultWalkSpeed();
		MovComp->MaxWalkSpeedCrouched = Data.NewValue * Player->GetDefaultCrouchSpeed();
	}
}

void APEPlayerState::JumpRateChanged_Callback(const FOnAttributeChangeData& Data) const
{
	PLAYERSTATE_VLOG(this, Warning, TEXT(" %s called with %f value"), TEXT(__FUNCTION__), Data.NewValue);

	const APECharacterBase* Player = Cast<APECharacterBase>(GetPawn());

	if (IsValid(Player))
	{
		UCharacterMovementComponent* MovComp = Player->GetCharacterMovement();
		MovComp->JumpZVelocity = Data.NewValue * Player->GetDefaultJumpVelocity();
	}
}

UAbilitySystemComponent* APEPlayerState::GetAbilitySystemComponent() const
{
	PLAYERSTATE_VLOG(this, Warning, TEXT(" %s called"), TEXT(__FUNCTION__));

	return AbilitySystemComponent.Get();
}

UAttributeSet* APEPlayerState::GetAttributeSetBase() const
{
	PLAYERSTATE_VLOG(this, Warning, TEXT(" %s called"), TEXT(__FUNCTION__));
	return Attributes.Get();
}

TArray<UAttributeSet*> APEPlayerState::GetAttributeSetArray() const
{
	PLAYERSTATE_VLOG(this, Warning, TEXT(" %s called"), TEXT(__FUNCTION__));

	return AbilitySystemComponent.Get()->GetSpawnedAttributes();
}

#define RETURN_ATTRIBUTE_LOGGED_VALUE(Attributes, PropertyName) \
{\
	const float OutValue = Attributes.IsValid() ? Attributes->Get##PropertyName() : -1.f;\
	PLAYERSTATE_VLOG(this, Warning, TEXT(" %s called with %f value"), TEXT(__FUNCTION__), OutValue);\
	return OutValue;\
}

float APEPlayerState::GetHealth() const
{
	RETURN_ATTRIBUTE_LOGGED_VALUE(Attributes, Health);
}

float APEPlayerState::GetMaxHealth() const
{
	RETURN_ATTRIBUTE_LOGGED_VALUE(Attributes, MaxHealth);
}

float APEPlayerState::GetStamina() const
{
	RETURN_ATTRIBUTE_LOGGED_VALUE(Attributes, Stamina);
}

float APEPlayerState::GetMaxStamina() const
{
	RETURN_ATTRIBUTE_LOGGED_VALUE(Attributes, MaxStamina);
}

float APEPlayerState::GetMana() const
{
	RETURN_ATTRIBUTE_LOGGED_VALUE(Attributes, Mana);
}

float APEPlayerState::GetMaxMana() const
{
	RETURN_ATTRIBUTE_LOGGED_VALUE(Attributes, MaxMana);
}

float APEPlayerState::GetAttackRate() const
{
	RETURN_ATTRIBUTE_LOGGED_VALUE(Attributes, AttackRate);
}

float APEPlayerState::GetDefenseRate() const
{
	RETURN_ATTRIBUTE_LOGGED_VALUE(Attributes, DefenseRate);
}

float APEPlayerState::GetSpeedRate() const
{
	RETURN_ATTRIBUTE_LOGGED_VALUE(Attributes, SpeedRate);
}

float APEPlayerState::GetJumpRate() const
{
	RETURN_ATTRIBUTE_LOGGED_VALUE(Attributes, JumpRate);
}