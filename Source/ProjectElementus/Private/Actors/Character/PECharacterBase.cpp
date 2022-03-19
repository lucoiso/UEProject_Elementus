// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Actors/Character/PECharacterBase.h"

#include "Actors/Character/PEPlayerController.h"
#include "Actors/Character/PEPlayerState.h"
#include "Camera/CameraComponent.h"

#include "Components/CapsuleComponent.h"
#include "Components/GameFrameworkComponentManager.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "GAS/System/GASAbilitySystemComponent.h"
#include "GAS/System/GASAttributeSet.h"

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
		TEXT("/Game/Main/Character/Mesh/SK_Mannequin"));
	if (SkeletalMesh.Object != nullptr)
	{
		GetMesh()->SetSkeletalMesh(SkeletalMesh.Object);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimationClass(
		TEXT("/Game/Main/Character/Animations/ThirdPerson_AnimBP"));
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

APEPlayerState* APECharacterBase::GetPEPlayerState() const
{
	return Cast<APEPlayerState>(GetPlayerState());
}

void APECharacterBase::PreInitializeComponents()
{
	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);

	Super::PreInitializeComponents();
}

void APECharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void APECharacterBase::PossessedBy(AController* InputController)
{
	Super::PossessedBy(InputController);
}

void APECharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
}

void APECharacterBase::BeginPlay()
{
	Super::BeginPlay();

	DefaultWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	DefaultCrouchSpeed = GetCharacterMovement()->MaxWalkSpeedCrouched;
	DefaultJumpVelocity = GetCharacterMovement()->JumpZVelocity;
}

void APECharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);

	Super::EndPlay(EndPlayReason);
}

void APECharacterBase::InitializeAttributes(const bool bOnRep)
{
	APEPlayerState* State = Cast<APEPlayerState>(GetPlayerState());

	if (IsValid(State))
	{
		AbilitySystemComponent = Cast<UGASAbilitySystemComponent>(State->GetAbilitySystemComponent());
		Attributes = Cast<UGASAttributeSet>(State->GetAttributeSetBase());

		if (AbilitySystemComponent.IsValid() && Attributes.IsValid())
		{
			bOnRep
				? AbilitySystemComponent->InitAbilityActorInfo(State, this)
				: State->GetAbilitySystemComponent()->InitAbilityActorInfo(State, this);
		}
	}

	if (bOnRep || !bIsFrameworkReady)
	{
		UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(
			this, UGameFrameworkComponentManager::NAME_GameActorReady);

		bIsFrameworkReady = true;
	}
}

void APECharacterBase::GiveAbility_Implementation(const TSubclassOf<UGameplayAbility> Ability, const FName InputId)
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || !IsValid(Ability))
	{
		return;
	}

	const FGameplayAbilitySpec* AbilitySpec = GetAbilitySystemComponent()->FindAbilitySpecFromClass(Ability);
	const uint32 InputID = InputIDEnumerationClass->GetValueByName(InputId, EGetByNameFlags::CheckAuthoredName);

	if (AbilitySpec != nullptr || InputID == INDEX_NONE)
	{
		return;
	}

	const FGameplayAbilitySpec& Spec = FGameplayAbilitySpec(*Ability, 1, InputID, this);

	AbilitySystemComponent->GiveAbility(Spec);

	if (AbilitySystemComponent->FindAbilitySpecFromHandle(Spec.Handle) != nullptr)
	{
		CharacterAbilities.Add(Ability);
	}
}

void APECharacterBase::RemoveAbility_Implementation(const TSubclassOf<UGameplayAbility> Ability)
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || CharacterAbilities.Num() <= 0 ||
		!IsValid(Ability))
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
	// TO DO
	Destroy();
}

bool APECharacterBase::Die_Validate()
{
	return true;
}