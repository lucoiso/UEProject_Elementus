// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Actors/Character/PEPlayerState.h"
#include "Actors/Character/PECharacterBase.h"
#include "Actors/Character/PEPlayerController.h"

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

	static const ConstructorHelpers::FObjectFinder<UDataTable> LevelingData_ObjRef(
		TEXT("/Game/Main/GAS/Data/DT_Leveling"));
#if __cplusplus > 201402L // Check if C++ > C++14
	if constexpr (&LevelingData_ObjRef.Object != nullptr)
#else
	if (&LevelingData_ObjRef.Object != nullptr)
#endif
	{
		LevelingData = LevelingData_ObjRef.Object;
	}

	static const ConstructorHelpers::FObjectFinder<UDataTable> AttributesMetaData_ObjRef(
		TEXT("/Game/Main/GAS/Data/DT_Character_ATB_Default"));
#if __cplusplus > 201402L // Check if C++ > C++14
	if constexpr (&AttributesMetaData_ObjRef.Object != nullptr)
#else
	if (&AttributesMetaData_ObjRef.Object != nullptr)
#endif
	{
		AttributesData = AttributesMetaData_ObjRef.Object;
	}

	static const ConstructorHelpers::FClassFinder<UGameplayEffect> DeathGameplayEffect_ClassRef(
		TEXT("/Game/Main/GAS/Effects/States/GE_Death"));
#if __cplusplus > 201402L // Check if C++ > C++14
	if constexpr (&DeathGameplayEffect_ClassRef.Class != nullptr)
#else
	if (&DeathGameplayEffect_ClassRef.Class != nullptr)
#endif
	{
		DeathEffect = DeathGameplayEffect_ClassRef.Class;
	}
}

void APEPlayerState::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void APEPlayerState::BeginPlay()
{
	PLAYERSTATE_VLOG(this, Display, TEXT(" %s called."), *FString(__func__));

	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(
		this, UGameFrameworkComponentManager::NAME_GameActorReady);

	Super::BeginPlay();

	if (ensureMsgf(AbilitySystemComponent.IsValid(), TEXT("%s have a invalid AbilitySystemComponent"), *GetName()))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attributes->GetSpeedRateAttribute()).
			AddUObject(this, &APEPlayerState::SpeedRateChanged_Callback);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attributes->GetJumpRateAttribute()).
			AddUObject(this, &APEPlayerState::JumpRateChanged_Callback);

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attributes->GetHealthAttribute()).
			AddUObject(this, &APEPlayerState::HealthChanged_Callback);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attributes->GetStaminaAttribute()).
			AddUObject(this, &APEPlayerState::StaminaChanged_Callback);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attributes->GetManaAttribute()).
			AddUObject(this, &APEPlayerState::ManaChanged_Callback);

		AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("State.Dead")),
			EGameplayTagEventType::NewOrRemoved).AddUObject(this, &APEPlayerState::DeathStateChanged_Callback);

		AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("State.Stunned")),
			EGameplayTagEventType::NewOrRemoved).AddUObject(this, &APEPlayerState::StunStateChanged_Callback);

		if (AttributesData.IsValid())
		{
			Attributes->InitFromMetaDataTable(AttributesData.Get());
		}

		if (LevelingData.IsValid())
		{
			const FGASLevelingData LevelingInfo = *LevelingData->FindRow<FGASLevelingData>(
				FName(*FString::FromInt(Attributes->GetLevel())), "");

#if __cplusplus > 201402L // Check if C++ > C++14
			if constexpr (&LevelingInfo != nullptr)
#else
			if (&LevelingInfo != nullptr)
#endif
			{
				NextLevelRequirement = LevelingInfo.ExperienceNeeded;

				AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
					Attributes->GetExperienceAttribute()).
					AddUObject(this, &APEPlayerState::ExperienceChanged_Callback);
			}
		}
	}
}

void APEPlayerState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);

	Super::EndPlay(EndPlayReason);
}

APEPlayerController* APEPlayerState::GetPEPlayerController() const
{
	return Cast<APEPlayerController>(GetOwner());
}

void APEPlayerState::DeathStateChanged_Callback(const FGameplayTag CallbackTag, const int32 NewCount) const
{
	if (!HasAuthority())
	{
		return;
	}

	PLAYERSTATE_VLOG(this, Display, TEXT(" %s called with %s Callback Tag and NewCount equal to %d"),
		*FString(__func__),
		*CallbackTag.ToString(), NewCount);

	if (NewCount != 0)
	{
		APEPlayerController* Controller_Temp = GetPEPlayerController();

		if (ensureMsgf(IsValid(Controller_Temp), TEXT("%s have a invalid Controller"), *GetName()))
		{
			APECharacterBase* Player_Temp = Controller_Temp->GetPawn<APECharacterBase>();

			if (ensureMsgf(IsValid(Player_Temp), TEXT("%s have a invalid Player"), *GetName()))
			{
				const FVector SpectatorLocation = Player_Temp->GetActorLocation();
				const FRotator SpectatorRotation = Player_Temp->GetActorRotation();

				Player_Temp->PerformDeath();

				Controller_Temp->ServerSetSpectatorLocation(SpectatorLocation, SpectatorRotation);
			}

			Controller_Temp->ChangeState(NAME_Spectating);
			Controller_Temp->ClientGotoState(NAME_Spectating);

			Controller_Temp->RemoveHUD();
		}
	}
}

void APEPlayerState::StunStateChanged_Callback(const FGameplayTag CallbackTag, const int32 NewCount) const
{
	PLAYERSTATE_VLOG(this, Display, TEXT(" %s called with %s Callback Tag and NewCount equal to %d"),
		*FString(__func__),
		*CallbackTag.ToString(), NewCount);

	if (ensureMsgf(IsValid(GetPlayerController()), TEXT("%s have a invalid Player"), *GetName()))
	{
		GetOwningController()->SetIgnoreMoveInput(NewCount != 0);
	}
}

void APEPlayerState::HealthChanged_Callback(const FOnAttributeChangeData& Data) const
{
	PLAYERSTATE_VLOG(this, Display, TEXT(" %s called with %f value"), *FString(__func__), Data.NewValue);

	if (Data.NewValue <= 0.f)
	{
		AbilitySystemComponent->CancelAllAbilities();

		AbilitySystemComponent->ApplyGameplayEffectToSelf(Cast<UGameplayEffect>(
			DeathEffect->GetDefaultObject()), 1.f, AbilitySystemComponent->MakeEffectContext());
	}
}

void APEPlayerState::StaminaChanged_Callback(const FOnAttributeChangeData& Data) const
{
	PLAYERSTATE_VLOG(this, Display, TEXT(" %s called with %f value"), *FString(__func__), Data.NewValue);

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
	PLAYERSTATE_VLOG(this, Display, TEXT(" %s called with %f value"), *FString(__func__), Data.NewValue);

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
	if (!IsValid(GetPawn()))
	{
		return;
	}

	PLAYERSTATE_VLOG(this, Display, TEXT(" %s called with %f value"), *FString(__func__), Data.NewValue);

	const APECharacterBase* Player = GetPawn<APECharacterBase>();

	if (ensureMsgf(IsValid(Player), TEXT("%s have a invalid Player"), *GetName()))
	{
		UCharacterMovementComponent* MovComp = Player->GetCharacterMovement();
		if (ensureMsgf(IsValid(MovComp), TEXT("%s have a invalid Movement Component"), *GetName()))
		{
			MovComp->MaxWalkSpeed = Data.NewValue * Player->GetDefaultWalkSpeed();
			MovComp->MaxWalkSpeedCrouched = Data.NewValue * Player->GetDefaultCrouchSpeed();
		}
	}
}

void APEPlayerState::JumpRateChanged_Callback(const FOnAttributeChangeData& Data) const
{
	if (!IsValid(GetPawn()))
	{
		return;
	}

	PLAYERSTATE_VLOG(this, Display, TEXT(" %s called with %f value"), *FString(__func__), Data.NewValue);

	const APECharacterBase* Player = GetPawn<APECharacterBase>();

	if (ensureMsgf(IsValid(Player), TEXT("%s have a invalid Player"), *GetName()))
	{
		UCharacterMovementComponent* MovComp = Player->GetCharacterMovement();
		if (ensureMsgf(IsValid(MovComp), TEXT("%s have a invalid Movement Component"), *GetName()))
		{
			MovComp->JumpZVelocity = Data.NewValue * Player->GetDefaultJumpVelocity();
		}
	}
}

UAbilitySystemComponent* APEPlayerState::GetAbilitySystemComponent() const
{
	PLAYERSTATE_VLOG(this, Display, TEXT(" %s called"), *FString(__func__));
	return AbilitySystemComponent.Get();
}

UAttributeSet* APEPlayerState::GetAttributeSetBase() const
{
	PLAYERSTATE_VLOG(this, Display, TEXT(" %s called"), *FString(__func__));
	return Attributes.Get();
}

TArray<UAttributeSet*> APEPlayerState::GetAttributeSetArray() const
{
	PLAYERSTATE_VLOG(this, Display, TEXT(" %s called"), *FString(__func__));
	return AbilitySystemComponent.Get()->GetSpawnedAttributes();
}

#define RETURN_ATTRIBUTE_LOGGED_VALUE(Attributes, PropertyName) \
{\
	const float OutValue = Attributes.IsValid() ? Attributes->Get##PropertyName() : -1.f;\
	PLAYERSTATE_VLOG(this, Display, TEXT(" %s called with %f value"), *FString(__func__), OutValue);\
	return OutValue;\
}

const float APEPlayerState::GetHealth() const
{
	RETURN_ATTRIBUTE_LOGGED_VALUE(Attributes, Health);
}

const float APEPlayerState::GetMaxHealth() const
{
	RETURN_ATTRIBUTE_LOGGED_VALUE(Attributes, MaxHealth);
}

const float APEPlayerState::GetStamina() const
{
	RETURN_ATTRIBUTE_LOGGED_VALUE(Attributes, Stamina);
}

const float APEPlayerState::GetMaxStamina() const
{
	RETURN_ATTRIBUTE_LOGGED_VALUE(Attributes, MaxStamina);
}

const float APEPlayerState::GetMana() const
{
	RETURN_ATTRIBUTE_LOGGED_VALUE(Attributes, Mana);
}

const float APEPlayerState::GetMaxMana() const
{
	RETURN_ATTRIBUTE_LOGGED_VALUE(Attributes, MaxMana);
}

const float APEPlayerState::GetAttackRate() const
{
	RETURN_ATTRIBUTE_LOGGED_VALUE(Attributes, AttackRate);
}

const float APEPlayerState::GetDefenseRate() const
{
	RETURN_ATTRIBUTE_LOGGED_VALUE(Attributes, DefenseRate);
}

const float APEPlayerState::GetSpeedRate() const
{
	RETURN_ATTRIBUTE_LOGGED_VALUE(Attributes, SpeedRate);
}

const float APEPlayerState::GetJumpRate() const
{
	RETURN_ATTRIBUTE_LOGGED_VALUE(Attributes, JumpRate);
}

const float APEPlayerState::GetExperience() const
{
	RETURN_ATTRIBUTE_LOGGED_VALUE(Attributes, Experience);
}

const float APEPlayerState::GetLevelingRequirementExp() const
{
	PLAYERSTATE_VLOG(this, Display, TEXT(" %s called"), *FString(__func__));

	return NextLevelRequirement;
}

const float APEPlayerState::GetGold() const
{
	RETURN_ATTRIBUTE_LOGGED_VALUE(Attributes, Gold);
}

#define UPDATE_ATTRIBUTE_INFORMATIONS(AttributeSet, AttributePropery, LevelingInfo) \
AttributeSet->AttributePropery = AttributeSet->Get##AttributePropery() + LevelingInfo.Bonus##AttributePropery;

void APEPlayerState::SetupCharacterLevel(const uint32 NewLevel)
{
	if (ensureMsgf(Attributes.IsValid(), TEXT("%s have a invalid AttributeSet"), *GetName()) &&
		ensureMsgf(LevelingData.IsValid(), TEXT("%s have a invalid LevelingData"), *GetName()))
	{
		const FGASLevelingData LevelingInfo = *LevelingData->FindRow<FGASLevelingData>(
			FName(*FString::FromInt(NewLevel)), "");

#if __cplusplus > 201402L // Check if C++ > C++14
		if constexpr (&LevelingInfo != nullptr)
#else
		if (&LevelingInfo != nullptr)
#endif
		{
			UPDATE_ATTRIBUTE_INFORMATIONS(Attributes, AttackRate, LevelingInfo);
			UPDATE_ATTRIBUTE_INFORMATIONS(Attributes, MaxHealth, LevelingInfo);
			UPDATE_ATTRIBUTE_INFORMATIONS(Attributes, MaxStamina, LevelingInfo);
			UPDATE_ATTRIBUTE_INFORMATIONS(Attributes, MaxMana, LevelingInfo);
			UPDATE_ATTRIBUTE_INFORMATIONS(Attributes, DefenseRate, LevelingInfo);

			const float NewExperience = Attributes->GetExperience() - NextLevelRequirement;
			NextLevelRequirement = LevelingInfo.ExperienceNeeded;

			Attributes->SetLevel(NewLevel);
			Attributes->SetExperience(NewExperience);
		}
		else // Reached max level
		{
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
				Attributes->GetExperienceAttribute()).RemoveAll(this);
		}
	}
}

void APEPlayerState::ExperienceChanged_Callback(const FOnAttributeChangeData& Data)
{
	if (Data.NewValue >= NextLevelRequirement)
	{
		SetupCharacterLevel(Attributes->GetLevel() + 1);
	}
}