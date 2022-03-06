// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Actors/Character/PEPlayerState.h"
#include "Actors/Character/PECharacterBase.h"

#include "GAS/System/GASAbilitySystemComponent.h"
#include "GAS/System/GASAttributeSet.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/GameFrameworkComponentManager.h"

#include "Management/Data/GASAbilityData.h"

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

	static ConstructorHelpers::FObjectFinder<UDataTable> LevelingDataObject(
		TEXT("/Game/Main/GAS/Data/DT_Leveling"));
	if (LevelingDataObject.Object != nullptr)
	{
		LevelingData = LevelingDataObject.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UDataTable> AttributesMetaDataObject(
		TEXT("/Game/Main/GAS/Data/DT_Character_ATB_Default"));
	if (AttributesMetaDataObject.Object != nullptr)
	{
		AttributesData = AttributesMetaDataObject.Object;
	}

	static ConstructorHelpers::FClassFinder<UGameplayEffect> DeathGameplayEffectClass(
		TEXT("/Game/Main/GAS/Effects/States/GE_Death"));
	if (DeathGameplayEffectClass.Class != nullptr)
	{
		DeathEffect = DeathGameplayEffectClass.Class;
	}
}

void APEPlayerState::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void APEPlayerState::BeginPlay()
{
	PLAYERSTATE_VLOG(this, Warning, TEXT(" %s called."), __func__);

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

		if (AttributesData.IsValid())
		{
			Attributes->InitFromMetaDataTable(AttributesData.Get());
		}

		if (LevelingData.IsValid())
		{
			const FGASLevelingData* LevelingInfo = LevelingData->FindRow<FGASLevelingData>(
				FName(*FString::FromInt(Attributes->GetLevel())), "");

			if (LevelingInfo != nullptr)
			{
				NextLevelRequirement = LevelingInfo->ExperienceNeeded;

				AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
					Attributes->GetExperienceAttribute()).
					AddUObject(
						this, &APEPlayerState::ExperienceChanged_Callback);
			}
		}
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
	                 __func__,
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
	                 __func__,
	                 *CallbackTag.ToString(), NewCount);

	const APECharacterBase* Player = Cast<APECharacterBase>(GetPawn());

	if (IsValid(Player) && IsValid(Player->GetController()))
	{
		Player->GetController()->SetIgnoreMoveInput(NewCount != 0);
	}
}

void APEPlayerState::HealthChanged_Callback(const FOnAttributeChangeData& Data) const
{
	PLAYERSTATE_VLOG(this, Warning, TEXT(" %s called with %f value"), __func__, Data.NewValue);

	if (Data.NewValue <= 0.f)
	{
		AbilitySystemComponent->CancelAllAbilities();
		AbilitySystemComponent->ApplyGameplayEffectToSelf(Cast<UGameplayEffect>(DeathEffect.LoadSynchronous()->GetDefaultObject()), 1.f,
			AbilitySystemComponent->MakeEffectContext());
	}
}

void APEPlayerState::StaminaChanged_Callback(const FOnAttributeChangeData& Data) const
{
	PLAYERSTATE_VLOG(this, Warning, TEXT(" %s called with %f value"), __func__, Data.NewValue);

	if (Data.NewValue == 0.f)
	{
		const FGameplayTagContainer StaminaCostTagContainer
		{
			FGameplayTag::RequestGameplayTag(FName("GameplayAbility.State.CostWhileActive.Stamina"))
		};

		AbilitySystemComponent->CancelAbilities(&StaminaCostTagContainer);
	}
}

void APEPlayerState::ManaChanged_Callback(const FOnAttributeChangeData& Data) const
{
	PLAYERSTATE_VLOG(this, Warning, TEXT(" %s called with %f value"), __func__, Data.NewValue);

	if (Data.NewValue == 0.f)
	{
		const FGameplayTagContainer ManaCostTagContainer
		{
			FGameplayTag::RequestGameplayTag(FName("GameplayAbility.State.CostWhileActive.Mana"))
		};

		AbilitySystemComponent->CancelAbilities(&ManaCostTagContainer);
	}
}

void APEPlayerState::SpeedRateChanged_Callback(const FOnAttributeChangeData& Data) const
{
	PLAYERSTATE_VLOG(this, Warning, TEXT(" %s called with %f value"), __func__, Data.NewValue);

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
	PLAYERSTATE_VLOG(this, Warning, TEXT(" %s called with %f value"), __func__, Data.NewValue);

	const APECharacterBase* Player = Cast<APECharacterBase>(GetPawn());

	if (IsValid(Player))
	{
		UCharacterMovementComponent* MovComp = Player->GetCharacterMovement();
		MovComp->JumpZVelocity = Data.NewValue * Player->GetDefaultJumpVelocity();
	}
}

UAbilitySystemComponent* APEPlayerState::GetAbilitySystemComponent() const
{
	PLAYERSTATE_VLOG(this, Warning, TEXT(" %s called"), __func__);

	return AbilitySystemComponent.Get();
}

UAttributeSet* APEPlayerState::GetAttributeSetBase() const
{
	PLAYERSTATE_VLOG(this, Warning, TEXT(" %s called"), __func__);
	return Attributes.Get();
}

TArray<UAttributeSet*> APEPlayerState::GetAttributeSetArray() const
{
	PLAYERSTATE_VLOG(this, Warning, TEXT(" %s called"), __func__);

	return AbilitySystemComponent.Get()->GetSpawnedAttributes();
}

#define RETURN_ATTRIBUTE_LOGGED_VALUE(Attributes, PropertyName) \
{\
	const float OutValue = Attributes.IsValid() ? Attributes->Get##PropertyName() : -1.f;\
	PLAYERSTATE_VLOG(this, Warning, TEXT(" %s called with %f value"), __func__, OutValue);\
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

float APEPlayerState::GetExperience() const
{
	RETURN_ATTRIBUTE_LOGGED_VALUE(Attributes, Experience);
}

float APEPlayerState::GetLevelingRequirementExp() const
{
	PLAYERSTATE_VLOG(this, Warning, TEXT(" %s called"), __func__);

	return NextLevelRequirement;
}

float APEPlayerState::GetGold() const
{
	RETURN_ATTRIBUTE_LOGGED_VALUE(Attributes, Gold);
}


#define UPDATE_ATTRIBUTE_INFORMATIONS(AttributeSet, AttributePropery, LevelingInfo) \
AttributeSet->AttributePropery = AttributeSet->Get##AttributePropery() + LevelingInfo->Bonus##AttributePropery;

void APEPlayerState::SetupCharacterLevel(const uint32 NewLevel)
{
	if (LevelingData.IsValid())
	{
		const FGASLevelingData* LevelingInfo = LevelingData->FindRow<FGASLevelingData>(
			FName(*FString::FromInt(NewLevel)), "");

		if (Attributes.IsValid() && LevelingInfo != nullptr)
		{
			UPDATE_ATTRIBUTE_INFORMATIONS(Attributes, AttackRate, LevelingInfo);
			UPDATE_ATTRIBUTE_INFORMATIONS(Attributes, MaxHealth, LevelingInfo);
			UPDATE_ATTRIBUTE_INFORMATIONS(Attributes, MaxStamina, LevelingInfo);
			UPDATE_ATTRIBUTE_INFORMATIONS(Attributes, MaxMana, LevelingInfo);
			UPDATE_ATTRIBUTE_INFORMATIONS(Attributes, DefenseRate, LevelingInfo);

			const float NewExperience = Attributes->GetExperience() - NextLevelRequirement;
			NextLevelRequirement = LevelingInfo->ExperienceNeeded;

			Attributes->SetLevel(NewLevel);
			Attributes->SetExperience(NewExperience);
		}
		else // Reached max level
		{
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attributes->GetExperienceAttribute()).
				RemoveAll(this);
		}
	}
}

void APEPlayerState::ExperienceChanged_Callback(const FOnAttributeChangeData& Data)
{
	if (Attributes.IsValid() && Data.NewValue >= NextLevelRequirement)
	{
		SetupCharacterLevel(Attributes->GetLevel() + 1);
	}
}