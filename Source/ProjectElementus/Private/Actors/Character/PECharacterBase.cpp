// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Actors/Character/PECharacterBase.h"
#include "Actors/Character/PEAIController.h"
#include "Actors/Character/PEPlayerState.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/GameFrameworkComponentManager.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "AbilitySystemComponent.h"

APECharacterBase::APECharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	AIControllerClass = APEAIController::StaticClass();

	bAlwaysRelevant = true;

	GetCapsuleComponent()->InitCapsuleSize(35.f, 90.0f);

	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetMobility(EComponentMobility::Movable);

	static const ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMesh_ObjRef(
		TEXT("/Game/Main/Character/Meshes/Manny_Quinn/SKM_Manny"));
	if constexpr (&SkeletalMesh_ObjRef.Object != nullptr)
	{
		GetMesh()->SetSkeletalMesh(SkeletalMesh_ObjRef.Object);
	}

	static const ConstructorHelpers::FClassFinder<UAnimInstance> Animation_ClassRef(
		TEXT("/Game/Main/Character/Animations/Blueprints/ABP_Manny"));
	if constexpr (&Animation_ClassRef.Class != nullptr)
	{
		GetMesh()->SetAnimInstanceClass(Animation_ClassRef.Class);
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
	GetCharacterMovement()->GravityScale = 1.25f;

	DefaultWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	DefaultCrouchSpeed = GetCharacterMovement()->MaxWalkSpeedCrouched;
	DefaultJumpVelocity = GetCharacterMovement()->JumpZVelocity;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 500.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = true;
	FollowCamera->SetRelativeLocation(FVector(50.f, 50.f, 50.f));
}

void APECharacterBase::PossessedBy(AController* InputController)
{
	Super::PossessedBy(InputController);

	InitializeABSC(false);
}

void APECharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitializeABSC(true);
}

void APECharacterBase::OnRep_Controller()
{
	Super::OnRep_Controller();

	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->RefreshAbilityActorInfo();

		const FGameplayTag DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
		AbilitySystemComponent->SetTagMapCount(DeadTag, 0);
	}
}

void APECharacterBase::InitializeABSC(const bool bOnRep)
{
	APEPlayerState* State = GetPlayerState<APEPlayerState>();

	if (IsValid(State))
	{
		AbilitySystemComponent = State->GetAbilitySystemComponent();

		if (ensureMsgf(AbilitySystemComponent.IsValid(), TEXT("%s have a invalid AbilitySystemComponent"), *GetName()))
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

const float APECharacterBase::GetDefaultWalkSpeed() const
{
	return DefaultWalkSpeed;
}

const float APECharacterBase::GetDefaultCrouchSpeed() const
{
	return DefaultCrouchSpeed;
}

const float APECharacterBase::GetDefaultJumpVelocity() const
{
	return DefaultJumpVelocity;
}

const FVector APECharacterBase::GetCameraForwardVector() const
{
	return FollowCamera->GetForwardVector();
}

const FVector APECharacterBase::GetCameraComponentLocation() const
{
	return FollowCamera->GetComponentLocation();
}

const float APECharacterBase::GetCameraTargetArmLength() const
{
	return CameraBoom->TargetArmLength;
}

UAbilitySystemComponent* APECharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

void APECharacterBase::PreInitializeComponents()
{
	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);

	Super::PreInitializeComponents();
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
	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->ClearAllAbilities();
	}

	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);

	Super::EndPlay(EndPlayReason);
}

void APECharacterBase::GiveAbility_Implementation(const TSubclassOf<UGameplayAbility> Ability, const FName InputId,
	const bool bTryRemoveExistingAbilityWithInput = true,
	const bool bTryRemoveExistingAbilityWithClass = true)
{
	if (ensureMsgf(AbilitySystemComponent.IsValid(), TEXT("%s have a invalid Ability System Component"), *GetName()))
	{
		if (GetLocalRole() != ROLE_Authority || !IsValid(Ability))
		{
			return;
		}

		const uint32 InputID = InputIDEnumerationClass->GetValueByName(InputId, EGetByNameFlags::CheckAuthoredName);
		if (InputID == INDEX_NONE)
		{
			return;
		}

		const auto RemoveAbility_Lambda = [&](const FGameplayAbilitySpec* AbilitySpec) -> void
		{
			if (AbilitySpec != nullptr)
			{
				RemoveAbility(AbilitySpec->Ability->GetClass());
			}
		};

		if (bTryRemoveExistingAbilityWithClass)
		{
			const FGameplayAbilitySpec* AbilitySpec = AbilitySystemComponent->FindAbilitySpecFromClass(Ability);
			RemoveAbility_Lambda(AbilitySpec);
		}

		if (bTryRemoveExistingAbilityWithInput)
		{
			const FGameplayAbilitySpec* AbilitySpec = AbilitySystemComponent->FindAbilitySpecFromInputID(InputID);
			RemoveAbility_Lambda(AbilitySpec);
		}

		const FGameplayAbilitySpec& Spec = FGameplayAbilitySpec(*Ability, 1, InputID, this);

		AbilitySystemComponent->GiveAbility(Spec);

		if (AbilitySystemComponent->FindAbilitySpecFromHandle(Spec.Handle) != nullptr)
		{
			CharacterAbilities.Add(Ability);
		}
	}
}

void APECharacterBase::RemoveAbility_Implementation(const TSubclassOf<UGameplayAbility> Ability)
{
	if (ensureMsgf(AbilitySystemComponent.IsValid(), TEXT("%s have a invalid Ability System Component"), *GetName()))
	{
		if (GetLocalRole() != ROLE_Authority || CharacterAbilities.Num() <= 0 ||
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
}

void APECharacterBase::PerformDeath_Implementation()
{
	// TO DO
	Destroy();
}

bool APECharacterBase::PerformDeath_Validate()
{
	return true;
}

void APECharacterBase::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (!AbilitySystemComponent.IsValid())
	{
		return;
	}

	const FGameplayTag DoubleJumpTag = FGameplayTag::RequestGameplayTag(FName("GameplayAbility.Default.DoubleJump"));

	if (AbilitySystemComponent->HasMatchingGameplayTag(DoubleJumpTag))
	{
		const FGameplayAbilitySpec AbilitySpec =
			*AbilitySystemComponent->FindAbilitySpecFromInputID(
				InputIDEnumerationClass->GetValueByName("Jump", EGetByNameFlags::CheckAuthoredName));

		if constexpr (&AbilitySpec != nullptr)
		{
			if (AbilitySpec.Handle.IsValid())
			{
				AbilitySystemComponent->CancelAbilityHandle(AbilitySpec.Handle);
			}
		}
	}
}