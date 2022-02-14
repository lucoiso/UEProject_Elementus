// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Actors/Character/PECharacterBase.h"

#include "Camera/CameraComponent.h"

#include "Components/CapsuleComponent.h"
#include "Components/GameFrameworkComponentManager.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "GAS/System/GASAbilitySystemComponent.h"
#include "GAS/System/GASAttributeSet.h"

#include "Management/Data/GASAbilityData.h"

APECharacterBase::APECharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	bAlwaysRelevant = true;

	GetCapsuleComponent()->InitCapsuleSize(35.f, 90.0f);

	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetMobility(EComponentMobility::Movable);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMesh(
		TEXT("/Game/Main/Mannequin/Character/Mesh/SK_Mannequin"));
	if (SkeletalMesh.Object != nullptr)
	{
		GetMesh()->SetSkeletalMesh(SkeletalMesh.Object);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimationClass(
		TEXT("/Game/Main/Mannequin/Animations/ThirdPerson_AnimBP"));
	if (AnimationClass.Class != nullptr)
	{
		GetMesh()->SetAnimInstanceClass(AnimationClass.Class);
	}

	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 300.f;
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.375f;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	DefaultWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	DefaultCrouchSpeed = GetCharacterMovement()->MaxWalkSpeedCrouched;
	DefaultJumpVelocity = GetCharacterMovement()->JumpZVelocity;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 500.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	FollowCamera->SetRelativeLocation(FVector(50.f, 50.f, 50.f));

	static ConstructorHelpers::FClassFinder<UGameplayEffect> DeathGameplayEffectClass(
		TEXT("/Game/Main/GAS/Effects/States/GE_Death"));
	if (DeathGameplayEffectClass.Class != nullptr)
	{
		DeathEffect = DeathGameplayEffectClass.Class;
	}
}

float APECharacterBase::GetDefaultWalkSpeed() const
{
	return DefaultWalkSpeed;
}

float APECharacterBase::GetDefaultCrouchSpeed() const
{
	return DefaultCrouchSpeed;
}

float APECharacterBase::GetDefaultJumpVelocity() const
{
	return DefaultJumpVelocity;
}

FVector APECharacterBase::GetCameraForwardVector() const
{
	return FollowCamera->GetForwardVector();
}

FVector APECharacterBase::GetCameraComponentLocation() const
{
	return FollowCamera->GetComponentLocation();
}

float APECharacterBase::GetCameraTargetArmLength() const
{
	return CameraBoom->TargetArmLength;
}

UAbilitySystemComponent* APECharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

UAttributeSet* APECharacterBase::GetAttributeSetBase() const
{
	return Attributes.Get();
}

TArray<UAttributeSet*> APECharacterBase::GetAttributeSetArray() const
{
	return AbilitySystemComponent.Get()->GetSpawnedAttributes();
}

void APECharacterBase::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void APECharacterBase::BeginPlay()
{
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(
		this, UGameFrameworkComponentManager::NAME_GameActorReady);

	Super::BeginPlay();

	DefaultWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	DefaultCrouchSpeed = GetCharacterMovement()->MaxWalkSpeedCrouched;
	DefaultJumpVelocity = GetCharacterMovement()->JumpZVelocity;

	if (AbilitySystemComponent.IsValid() && Attributes.IsValid())
	{
		const FGASLevelingData* NextLevelingInfo = LevelingData->FindRow<FGASLevelingData>(
			FName(*FString::FromInt(Attributes->GetLevel() + 1)), "");

		if (NextLevelingInfo != nullptr)
		{
			NextLevelRequirement = NextLevelingInfo->ExperienceNeeded;

			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attributes->GetExperienceAttribute()).
			                        AddUObject(
				                        this, &APECharacterBase::ExperienceChanged_Callback);
		}

		if (!AttributesData.IsNull())
		{
			Attributes->InitFromMetaDataTable(AttributesData.LoadSynchronous());
		}
	}
}

void APECharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);

	Super::EndPlay(EndPlayReason);
}

void APECharacterBase::ExperienceChanged_Callback(const FOnAttributeChangeData& Data)
{
	if (Attributes.IsValid() && Data.NewValue >= NextLevelRequirement)
	{
		SetupCharacterLevel(Attributes->GetLevel() + 1);
	}
}

#define UPDATE_ATTRIBUTE_INFORMATIONS(AttributeSet, AttributePropery, LevelingInfo) \
AttributeSet->AttributePropery = AttributeSet->Get##AttributePropery() + LevelingInfo->Bonus##AttributePropery;

void APECharacterBase::SetupCharacterLevel_Implementation(const uint32 NewLevel)
{
	if (IsValid(LevelingData))
	{
		const FGASLevelingData* RespectiveLevelingInfo = LevelingData->FindRow<FGASLevelingData>(
			FName(*FString::FromInt(NewLevel - 1)), "");

		if (Attributes.IsValid() && RespectiveLevelingInfo != nullptr)
		{
			UPDATE_ATTRIBUTE_INFORMATIONS(Attributes, AttackRate, RespectiveLevelingInfo);
			UPDATE_ATTRIBUTE_INFORMATIONS(Attributes, MaxHealth, RespectiveLevelingInfo);
			UPDATE_ATTRIBUTE_INFORMATIONS(Attributes, MaxStamina, RespectiveLevelingInfo);
			UPDATE_ATTRIBUTE_INFORMATIONS(Attributes, MaxMana, RespectiveLevelingInfo);
			UPDATE_ATTRIBUTE_INFORMATIONS(Attributes, DefenseRate, RespectiveLevelingInfo);

			const FGASLevelingData* NextLevelingInfo = LevelingData->FindRow<FGASLevelingData>(
				FName(*FString::FromInt(NewLevel)), "");

			if (NextLevelingInfo != nullptr)
			{
				const float NewExperience = Attributes->GetExperience() - NextLevelRequirement;
				NextLevelRequirement = NextLevelingInfo->ExperienceNeeded;

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
}

bool APECharacterBase::SetupCharacterLevel_Validate(const uint32 NewLevel)
{
	return true;
}

void APECharacterBase::GiveAbility_Implementation(const TSubclassOf<UGameplayAbility> Ability)
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() ||
		CharacterAbilities.Num() >= 3 || !IsValid(Ability))
	{
		return;
	}

	const FGameplayAbilitySpec* AbilitySpec = GetAbilitySystemComponent()->FindAbilitySpecFromClass(Ability);

	if (AbilitySpec != nullptr)
	{
		return;
	}

	const int32& Index = CharacterAbilities.Num();

	auto InputID = [Index, this]() -> const uint32
	{
		switch (Index)
		{
		case (0):
			return InputIDEnumerationClass->GetValueByName("Skill 1", EGetByNameFlags::CheckAuthoredName);

		case(1):
			return InputIDEnumerationClass->GetValueByName("Skill 2", EGetByNameFlags::CheckAuthoredName);

		case(2):
			return InputIDEnumerationClass->GetValueByName("Skill 3", EGetByNameFlags::CheckAuthoredName);

		default:
			return -1;
		}
	};

	const FGameplayAbilitySpec& Spec = FGameplayAbilitySpec(*Ability, 1, InputID(), this);

	AbilitySystemComponent->GiveAbility(Spec);

	if (AbilitySystemComponent->FindAbilitySpecFromHandle(Spec.Handle) != nullptr)
	{
		CharacterAbilities.Add(Ability);
	}
}

void APECharacterBase::RemoveAbility_Implementation(const TSubclassOf<UGameplayAbility> Ability)
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || CharacterAbilities.Num() <= 0 || !
		IsValid(Ability))
	{
		return;
	}

	const FGameplayAbilitySpec* AbilitySpec = AbilitySystemComponent->FindAbilitySpecFromClass(Ability);

	if (AbilitySpec == nullptr)
	{
		return;
	}

	AbilitySystemComponent->ClearAbility(AbilitySpec->Handle);

	if (AbilitySystemComponent->FindAbilitySpecFromClass(Ability) == nullptr)
	{
		CharacterAbilities.Remove(Ability);
	}
}

void APECharacterBase::Die_Implementation()
{
	const FGameplayTag DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));

	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || !AbilitySystemComponent->
		HasMatchingGameplayTag(DeadTag))
	{
		return;
	}


	AbilitySystemComponent->CancelAllAbilities();
	AbilitySystemComponent->ApplyGameplayEffectToSelf(Cast<UGameplayEffect>(DeathEffect->GetDefaultObject()), 1.0f,
	                                                  AbilitySystemComponent->MakeEffectContext());

	FinishDying();
}

void APECharacterBase::FinishDying()
{
	Destroy();
}
