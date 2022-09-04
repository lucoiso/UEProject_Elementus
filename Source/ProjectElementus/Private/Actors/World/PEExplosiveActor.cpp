// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Actors/World/PEExplosiveActor.h"
#include "Actors/Character/PECharacter.h"
#include "GAS/System/PEAbilitySystemComponent.h"
#include "Components/PrimitiveComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"

APEExplosiveActor::APEExplosiveActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	  ExplosionRadius(150.f),
	  ExplosionMagnitude(1000.f),
	  bDestroyAfterExplosion(true),
	  bDebug(false)
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

	if (bDebug)
	{
		const FName TraceTag("SphereTraceDebugTag");
		GetWorld()->DebugDrawTraceTag = TraceTag;
		QueryParams.TraceTag = TraceTag;
	}

	GetWorld()->SweepMultiByObjectType(HitOut,
	                                   GetActorLocation(),
	                                   GetActorLocation(),
	                                   FQuat(FRotator(0.f)),
	                                   FCollisionObjectQueryParams::AllDynamicObjects,
	                                   FCollisionShape::MakeSphere(ExplosionRadius),
	                                   QueryParams);

	for (UNiagaraSystem* const& NiagaraSystem : ExplosionVFXs)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraSystem, GetActorLocation());
	}

	for (const FHitResult& Hit : HitOut)
	{
		if (IsValid(Hit.GetActor()))
		{
			const FVector Velocity =
				ExplosionMagnitude * (Hit.GetActor()->GetActorLocation() - GetActorLocation()).GetSafeNormal();

			if (Hit.GetActor()->GetClass()->IsChildOf<APECharacter>())
			{
				if (APECharacter* Player = Cast<APECharacter>(Hit.GetActor()))
				{
					Player->LaunchCharacter(Velocity, true, true);

					if (ensureAlwaysMsgf(IsValid(Player->GetAbilitySystemComponent()),
					                     TEXT("%s have a invalid Ability System Component"),
					                     *Player->GetName()))
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
	if (UPEAbilitySystemComponent* TargetGASC = Cast<UPEAbilitySystemComponent>(TargetABSC))
	{
		for (const FGameplayEffectGroupedData& Effect : ExplosionEffects)
		{
			TargetGASC->ApplyEffectGroupedDataToSelf(Effect);
		}
	}
}
