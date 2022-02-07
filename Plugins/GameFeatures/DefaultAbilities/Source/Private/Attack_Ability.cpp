#include "Attack_Ability.h"
#include "Actors/Character/PECharacterBase.h"
#include "GAS/System/GASAbilitySystemComponent.h"

UAttack_Ability::UAttack_Ability(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.Default.Attack"));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.Default.Attack"));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayEffect.Debuff.Regeneration.Block.Stamina"));
}

void UAttack_Ability::ActivateAbility
(const FGameplayAbilitySpecHandle Handle,
 const FGameplayAbilityActorInfo* ActorInfo,
 const FGameplayAbilityActivationInfo ActivationInfo,
 const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	FGASTraceDataHandle AbilityTraceDataHandle;
	AbilityTraceDataHandle.bDoTick = false;
	AbilityTraceDataHandle.Type = EGASTraceType::Single;
	AbilityTraceDataHandle.FilterHandle.Filter->RequiredActorClass = APECharacterBase::StaticClass();

	DoAbilityLineTrace(AbilityTraceDataHandle, ActorInfo->AvatarActor.Get());

	if (!IsValid(AbilityTraceDataHandle.Hit.GetActor()) ||
		AbilityTraceDataHandle.Hit.Location == FVector(0.f))
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	const APECharacterBase* Target = Cast<APECharacterBase>(AbilityTraceDataHandle.Hit.GetActor());

	if (IsValid(Target))
	{
		ApplyAbilityEffectsToTarget(MakeTargetDataHandleFromSingleHitResult(AbilityTraceDataHandle.Hit), Handle,
		                            ActorInfo, ActivationInfo);
	}
}

void UAttack_Ability::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
