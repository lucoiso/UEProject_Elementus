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

#ifdef UE_BUILD_DEBUG
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
		APECharacterBase* Player = Cast<APECharacterBase>(Hit.GetActor());

		if (IsValid(Player))
		{
			Player->LaunchCharacter(Velocity, false, false);
			for (const TSubclassOf<UGameplayEffect>& Effect : ExplosionEffects)
			{
				if (Player->HasAuthority())
				{
					Player->GetAbilitySystemComponent()->ApplyGameplayEffectToSelf(Effect.GetDefaultObject(),
						1.f, Player->GetAbilitySystemComponent()->MakeEffectContext());
				}
			}
		}

		else if (Hit.GetActor()->IsRootComponentMovable())
		{
			Hit.GetComponent()->AddForce(Velocity);
			Hit.GetComponent()->AddImpulse(Velocity);
		}
	}

	Destroy();
}
