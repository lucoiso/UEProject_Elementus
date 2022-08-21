// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Actors/Character/PECharacter.h"
#include "Actors/Character/PEAIController.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "AbilitySystemLog.h"
#include "ElementusInventoryComponent.h"
#include "ElementusInventoryFunctions.h"
#include "Actors/Character/PEPlayerState.h"
#include "Actors/Interfaces/PEEquipment.h"
#include "GAS/System/PEAbilitySystemComponent.h"
#include "Actors/World/PEInventoryPackage.h"

static const FGameplayTag& GenericEquipTag = FGameplayTag::RequestGameplayTag(FName("EquipSlot.Generic.Equipped"));

APECharacter::APECharacter(const FObjectInitializer& ObjectInitializer)
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
		TEXT("/Game/Main/Character/Meshes/Manny_Quinn/SKM_Manny_Simple"));
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

	InventoryComponent = CreateDefaultSubobject<UElementusInventoryComponent>(TEXT("InventoryComponent"));
	InventoryComponent->SetIsReplicated(true);
}

// Called on server when the player is possessed by a controller
void APECharacter::PossessedBy(AController* InController)
{
	Super::PossessedBy(InController);

	// Check if this character is controlled by a player or AI
	if (InController->IsPlayerController())
	{
		// Initialize the ability system component that is stored by Player State
		if (APEPlayerState* State = GetPlayerStateChecked<APEPlayerState>())
		{
			InitializeAbilitySystemComponent(State->GetAbilitySystemComponent(), State);
		}
	}
	else
	{
		// Bot / AI
		// Not implemented yet

		// Initialize the ability system component that is stored by AI Controller
	}
}

// Called on client when the player state is initialized
void APECharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (APEPlayerState* State = GetPlayerState<APEPlayerState>())
	{
		// Initialize the ability system component that is stored by Player State
		InitializeAbilitySystemComponent(State->GetAbilitySystemComponent(), State);
	}
}

// Called when controller is replicated, will use it to refresh the actor info on ABSC and reset the Death Tag state
void APECharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->RefreshAbilityActorInfo();
		AbilitySystemComponent->RemoveActiveEffectsWithTags(
			FGameplayTagContainer(FGameplayTag::RequestGameplayTag(TEXT("State.Dead"))));
	}
}

#pragma region Default Getters
float APECharacter::GetDefaultWalkSpeed() const
{
	return DefaultWalkSpeed;
}

float APECharacter::GetDefaultCrouchSpeed() const
{
	return DefaultCrouchSpeed;
}

float APECharacter::GetDefaultJumpVelocity() const
{
	return DefaultJumpVelocity;
}

FVector APECharacter::GetCameraForwardVector() const
{
	return FollowCamera->GetForwardVector();
}

FVector APECharacter::GetCameraComponentLocation() const
{
	return FollowCamera->GetComponentLocation();
}

float APECharacter::GetCameraTargetArmLength() const
{
	return CameraBoom->TargetArmLength;
}
#pragma endregion Default Getters

UAbilitySystemComponent* APECharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

void APECharacter::InitializeAbilitySystemComponent(UAbilitySystemComponent* InABSC, AActor* InOwnerActor)
{
	AbilitySystemComponent = CastChecked<UPEAbilitySystemComponent>(InABSC);
	AbilitySystemComponent->InitAbilityActorInfo(InOwnerActor, this);

	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(
		this, UGameFrameworkComponentManager::NAME_GameActorReady);
}

// ReSharper disable once CppUE4BlueprintCallableFunctionMayBeConst
void APECharacter::EquipItem(const FElementusItemInfo InItem, const FGameplayTag EquipmentSlotTag)
{
	if (!HasAuthority())
	{
		return;
	}

	if (int32 FoundIndex;
		InventoryComponent->FindElementusItemInStack(InItem,
		                                             FoundIndex,
		                                             FGameplayTagContainer::CreateFromArray(TArray{EquipmentSlotTag})))
	{
		if (InventoryComponent->GetItemReferenceAt(FoundIndex).Tags.HasTag(GenericEquipTag))
		{
			// Already equipped
			UnnequipItem(InItem, EquipmentSlotTag);
			return;
		}

		const FGameplayTagContainer& EquipTags = FGameplayTagContainer::CreateFromArray(TArray{
			GenericEquipTag, EquipmentSlotTag
		});
		InventoryComponent->GetItemReferenceAt(FoundIndex).Tags.AppendTags(EquipTags);

		if (const UElementusItemData* ItemData =
			UElementusInventoryFunctions::GetElementusItemDataById(InItem.ItemId, {"SoftData"}))
		{
			if (UPEEquipment* EquipedItem =
				Cast<UPEEquipment>(ItemData->ItemClass.LoadSynchronous()->GetDefaultObject()))
			{
				EquipedItem->ProcessEquipmentApplication(this);
			}
		}
	}
}

// ReSharper disable once CppUE4BlueprintCallableFunctionMayBeConst
void APECharacter::UnnequipItem(const FElementusItemInfo InItem, const FGameplayTag EquipmentSlotTag)
{
	if (!HasAuthority())
	{
		return;
	}

	if (int32 FoundIndex;
		InventoryComponent->FindElementusItemInStack(InItem,
		                                             FoundIndex,
		                                             FGameplayTagContainer::CreateFromArray(TArray{EquipmentSlotTag})))
	{
		const FGameplayTagContainer& EquipTags = FGameplayTagContainer::CreateFromArray(TArray{
			GenericEquipTag, EquipmentSlotTag
		});
		InventoryComponent->GetItemReferenceAt(FoundIndex).Tags.RemoveTags(EquipTags);

		if (const UElementusItemData* ItemData =
			UElementusInventoryFunctions::GetElementusItemDataById(InItem.ItemId, {"SoftData"}))
		{
			if (UPEEquipment* EquipedItem =
				Cast<UPEEquipment>(ItemData->ItemClass.LoadSynchronous()->GetDefaultObject()))
			{
				EquipedItem->ProcessEquipmentRemoval(this);
			}
		}
	}
}

void APECharacter::PreInitializeComponents()
{
	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);

	Super::PreInitializeComponents();
}

void APECharacter::BeginPlay()
{
	Super::BeginPlay();

	DefaultWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	DefaultCrouchSpeed = GetCharacterMovement()->MaxWalkSpeedCrouched;
	DefaultJumpVelocity = GetCharacterMovement()->JumpZVelocity;

	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->AbilityActivatedCallbacks.AddUFunction(this, "AbilityActivated");
		AbilitySystemComponent->AbilityCommittedCallbacks.AddUFunction(this, "AbilityCommited");
		AbilitySystemComponent->OnAbilityEnded.AddUFunction(this, "AbilityEnded");
		AbilitySystemComponent->AbilityFailedCallbacks.AddUFunction(this, "AbilityFailed");
	}

	// Check if this character have a valid Skeletal Mesh and paint it
	if (IsValid(GetMesh()))
	{
		const auto DynamicColor_Lambda = [&](const uint8 Index, const FLinearColor Color) -> void
		{
			if (UMaterialInstanceDynamic* DynMat = GetMesh()->CreateDynamicMaterialInstance(Index))
			{
				DynMat->SetVectorParameterValue(TEXT("Tint"), Color);
			}
		};

		// Bot: Red | Player: Blue
		const FLinearColor DestColor =
			IsBotControlled()
				? FLinearColor::Red
				: FLinearColor::Blue;

		DynamicColor_Lambda(0, DestColor);
		DynamicColor_Lambda(1, DestColor);
	}
}

void APECharacter::PerformDeath()
{
	OnCharacterDeath.Broadcast();

	Server_SpawnInventoryPackage();
	Multicast_DeathSetup();

	bAlwaysRelevant = false;

	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([&]
	{
		if (IsValid(this))
		{
			Server_PerformDeath();
		}
	});

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 15.0f, false);
}

void APECharacter::Server_PerformDeath_Implementation()
{
	// Destroy the character only on server (Will replicate to clients)
	Destroy();
}

void APECharacter::Multicast_DeathSetup_Implementation()
{
	// Will perform each step above on both server and client
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);

	if (IsValid(GetMesh()) && IsValid(GetCharacterMovement()) && IsValid(GetCapsuleComponent()))
	{
		GetCharacterMovement()->DisableMovement();
		GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
		GetCapsuleComponent()->SetCollisionProfileName(TEXT("NoCollision"));
		GetMesh()->SetAllBodiesBelowSimulatePhysics(NAME_None, true, true);
	}
}

void APECharacter::Server_SpawnInventoryPackage_Implementation()
{
	AElementusInventoryPackage* SpawnedPackage =
		GetWorld()->SpawnActorDeferred<APEInventoryPackage>(APEInventoryPackage::StaticClass(),
		                                                    GetTransform(),
		                                                    nullptr,
		                                                    nullptr,
		                                                    ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	UElementusInventoryFunctions::TradeElementusItem(InventoryComponent->GetItemStack(),
	                                                 InventoryComponent,
	                                                 SpawnedPackage->PackageInventory);

	SpawnedPackage->SetDestroyOnEmpty(true);
	SpawnedPackage->FinishSpawning(GetTransform());
}

void APECharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	// Check if this player have a valid ABSC and cancel the Double Jump ability (if active)
	if (!AbilitySystemComponent.IsValid())
	{
		return;
	}

	const FGameplayTagContainer DoubleJumpTagContainer
	{
		FGameplayTag::RequestGameplayTag(FName("GameplayAbility.Default.DoubleJump"))
	};

	AbilitySystemComponent->CancelAbilities(&DoubleJumpTagContainer);
}

void APECharacter::AbilityFailed_Implementation(const UGameplayAbility* Ability,
                                                const FGameplayTagContainer& TagContainer)
{
	ABILITY_VLOG(Ability, Warning,
	             TEXT("Ability %s failed to activate. Owner: %s"), *Ability->GetName(), *GetName());

	for (const auto& TagIterator : TagContainer)
	{
		if (TagIterator.IsValid())
		{
			ABILITY_VLOG(Ability, Warning, TEXT("Tag: %s"), *TagIterator.ToString());
		}
	}

#if WITH_EDITOR
	if (bPrintAbilityFailure)
	{
		ABILITY_VLOG(Ability, Warning,
		             TEXT("================ START OF ABILITY SYSTEM COMPONENT DEBUG INFO ================"));

		AbilitySystemComponent->PrintDebug();

		ABILITY_VLOG(Ability, Warning,
		             TEXT("================ END OF ABILITY SYSTEM COMPONENT DEBUG INFO ================"));
	}
#endif
}
