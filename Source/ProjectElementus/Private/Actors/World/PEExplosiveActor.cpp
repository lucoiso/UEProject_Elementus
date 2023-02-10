// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Actors/World/PEExplosiveActor.h"
#include "Actors/Character/PECharacter.h"
#include "Core/PEAbilitySystemComponent.h"
#include "Core/PEAbilityData.h"
#include <NiagaraFunctionLibrary.h>
#include <NiagaraSystem.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(PEExplosiveActor)

APEExplosiveActor::APEExplosiveActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), ExplosionRadius(150.f), ExplosionMagnitude(1000.f), bDestroyAfterExplosion(true)
{
	bReplicates = false;
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void APEExplosiveActor::PerformExplosion()
{
	// Only call SetReplicates if has authority
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	// Only replicates while exploding
	SetReplicates(true);

	TArray<FHitResult> HitOut;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.MobilityType = EQueryMobilityType::Dynamic;

#if UE_ENABLE_DEBUG_DRAWING
	const FName TraceTag("SphereTraceDebugTag");
	GetWorld()->DebugDrawTraceTag = TraceTag;
	QueryParams.TraceTag = TraceTag;
#endif

	GetWorld()->SweepMultiByObjectType(HitOut, GetActorLocation(), GetActorLocation(), FQuat(FRotator(0.f)), FCollisionObjectQueryParams::AllDynamicObjects, FCollisionShape::MakeSphere(ExplosionRadius), QueryParams);

	for (UNiagaraSystem* const& NiagaraSystem : ExplosionVFXs)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraSystem, GetActorLocation());
	}

	for (const FHitResult& Hit : HitOut)
	{
		if (IsValid(Hit.GetActor()))
		{
			const FVector Velocity = ExplosionMagnitude * (Hit.GetActor()->GetActorLocation() - GetActorLocation()).GetSafeNormal();

			if (Hit.GetActor()->GetClass()->IsChildOf<APECharacter>())
			{
				if (APECharacter* const Player = Cast<APECharacter>(Hit.GetActor()))
				{
					Player->LaunchCharacter(Velocity, true, true);

					if (ensureAlwaysMsgf(IsValid(Player->GetAbilitySystemComponent()), TEXT("%s have a invalid Ability System Component"), *Player->GetName()))
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
	}

	if (bDestroyAfterExplosion)
	{
		Destroy();
		return;
	}

	// Only replicates while exploding
	SetReplicates(false);
}

void APEExplosiveActor::ApplyExplosibleEffect(UAbilitySystemComponent* TargetABSC)
{
	if (UPEAbilitySystemComponent* const TargetGASC = Cast<UPEAbilitySystemComponent>(TargetABSC))
	{
		for (const FGameplayEffectGroupedData& Effect : ExplosionEffects)
		{
			TargetGASC->ApplyEffectGroupedDataToSelf(Effect);
		}
	}
}
