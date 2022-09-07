// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "PEDoubleJumpAbility.h"
#include "Actors/Character/PECharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Management/Data/PEGlobalTags.h"

UPEDoubleJumpAbility::UPEDoubleJumpAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.Default.DoubleJump"));

	ActivationOwnedTags.AddTag(GlobalTag_RegenBlock_Stamina);
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.Default.Dash"));

	static const ConstructorHelpers::FObjectFinder<USoundBase> ImpulseSound_ObjRef(TEXT("/DefaultAbilities/Sounds/MS_DoubleJump"));
	if constexpr (&ImpulseSound_ObjRef.Object != nullptr)
	{
		ImpulseSound = ImpulseSound_ObjRef.Object;
	}
}

void UPEDoubleJumpAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo,
                                           const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	APECharacter* const Player = Cast<APECharacter>(ActorInfo->AvatarActor.Get());

	// Only characters can activate this ability
	if (!IsValid(Player))
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	// Check if the player is in air and launch him (second jump) or just do a normal jump (first jump)
	if (!Player->GetCharacterMovement()->IsFalling())
	{
		Player->Jump();
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
	else
	{
		const FVector VFXLocation = Player->GetMesh()->GetSocketLocation("Pelvis_Socket");

		FGameplayCueParameters Params;
		Params.Location = VFXLocation;
		ActivateGameplayCues(FGameplayTag::RequestGameplayTag("GameplayCue.Default.DoubleJump"),
		                     Params,
		                     ActorInfo->AbilitySystemComponent.Get());

		UGameplayStatics::SpawnSoundAtLocation(ActorInfo->AvatarActor.Get(),
		                                       ImpulseSound,
		                                       VFXLocation,
		                                       FRotator::ZeroRotator,
		                                       0.3f);

		Player->LaunchCharacter(FVector(0.f, 0.f, AbilityMaxRange), false, true);
	}
}

void UPEDoubleJumpAbility::InputReleased(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo,
                                         const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	// Send the StopJumping event to the player if valid
	if (APECharacter* const Player = Cast<APECharacter>(ActorInfo->AvatarActor.Get()))
	{
		Player->StopJumping();
	}
}
