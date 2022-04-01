// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Actors/World/ExplosiveActor.h"
#include "Actors/Character/PECharacterBase.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"

AExplosiveActor::AExplosiveActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void AExplosiveActor::PerformExplosion()
{
	TArray<FHitResult> HitOut;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.MobilityType = EQueryMobilityType::Dynamic;

#if UE_BUILD_DEBUG
	const FName TraceTag("SphereTraceDebugTag");
	GetWorld()->DebugDrawTraceTag = TraceTag;
	QueryParams.TraceTag = TraceTag;
#endif // !UE_BUILD_DEBUG

	GetWorld()->SweepMultiByObjectType(HitOut,
		GetActorLocation(),
		GetActorLocation(),
		FQuat(FRotator(0.f)),
		FCollisionObjectQueryParams::AllDynamicObjects,
		FCollisionShape::MakeSphere(ExplosionRadius),
		QueryParams);

	for (UNiagaraSystem* NiagaraSystem : ExplosionVFXs)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(), NiagaraSystem, GetActorLocation());
	}

	for (const FHitResult& Hit : HitOut)
	{
		const FVector Velocity = ExplosionMagnitude * (Hit.GetActor()->GetActorLocation() - GetActorLocation());

		if (Hit.GetActor()->GetClass()->IsChildOf<APECharacterBase>())
		{
			APECharacterBase* Player = Cast<APECharacterBase>(Hit.GetActor());

			if (ensureMsgf(IsValid(Player), TEXT("%s have a invalid Player"), *GetActorLabel()))
			{
				Player->LaunchCharacter(Velocity, false, false);
				
				if (ensureMsgf(IsValid(Player->GetAbilitySystemComponent()), TEXT("%s have a invalid Ability System Component"), *Player->GetActorLabel()))
				{
					ApplyExplosibleEffect(Player->GetAbilitySystemComponent());
				}
			}
		}

		else if (Hit.GetActor()->IsRootComponentMovable())
		{
			Hit.GetComponent()->AddForce(Velocity);
			Hit.GetComponent()->AddImpulse(Velocity);
		}
	}

	if (bDestroyAfterExplosion)
	{
		Destroy();
	}
}

void AExplosiveActor::ApplyExplosibleEffect_Implementation(UAbilitySystemComponent* TargetComp)
{
	if (ensureMsgf(IsValid(TargetComp), TEXT("%s have a invalid target"), *GetActorLabel()))
	{
		if (GetLocalRole() != ROLE_Authority)
		{
			return;
		}

		for (const TSubclassOf<UGameplayEffect>& Effect : ExplosionEffects)
		{
			TargetComp->ApplyGameplayEffectToSelf(Effect.GetDefaultObject(),
				1.f, TargetComp->MakeEffectContext());
		}
	}
}

bool AExplosiveActor::ApplyExplosibleEffect_Validate(UAbilitySystemComponent* TargetComp)
{
	return false;
}
