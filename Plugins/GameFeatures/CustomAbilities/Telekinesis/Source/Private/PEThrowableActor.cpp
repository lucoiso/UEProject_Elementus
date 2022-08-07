// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "PEThrowableActor.h"
#include "Components/StaticMeshComponent.h"
#include "Actors/Character/PECharacter.h"
#include "GAS/System/PEAbilitySystemComponent.h"

APEThrowableActor::APEThrowableActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bNetStartup = false;
	bNetLoadOnClient = false;
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SetRootComponent(GetStaticMeshComponent());
	SetMobility(EComponentMobility::Movable);

	GetStaticMeshComponent()->SetSimulatePhysics(true);
	GetStaticMeshComponent()->SetGenerateOverlapEvents(true);
	GetStaticMeshComponent()->SetNotifyRigidBodyCollision(true);
	GetStaticMeshComponent()->SetCollisionObjectType(ECC_PhysicsBody);
	GetStaticMeshComponent()->SetCollisionProfileName(TEXT("PhysicsBody"));
	GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	bReplicates = true;
	GetStaticMeshComponent()->SetIsReplicated(true);
}

void APEThrowableActor::ThrowSetup(AActor* Caller)
{
	CallerActor.Reset();
	CallerActor = Caller;

	GetStaticMeshComponent()->OnComponentHit.AddDynamic(this, &APEThrowableActor::OnThrowableHit);
}

void APEThrowableActor::OnThrowableHit([[maybe_unused]] UPrimitiveComponent*, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, const FVector NormalImpulse,
                                       const FHitResult& Hit)
{
	if (IsValid(OtherActor) && OtherActor != CallerActor.Get())
	{
		if (OtherActor->GetClass()->IsChildOf<APECharacter>())
		{
			if (APECharacter* Player = Cast<APECharacter>(OtherActor))
			{
				constexpr float ImpulseMultiplier = 5.f;

				Player->LaunchCharacter(NormalImpulse.GetSafeNormal() * ImpulseMultiplier, false, false);

				if (ensureAlwaysMsgf(IsValid(Player->GetAbilitySystemComponent()),
				                     TEXT("%s have a invalid Ability System Component"), *Player->GetName()))
				{
					ApplyThrowableEffect(Player->GetAbilitySystemComponent());
				}
			}
		}
		else if (IsValid(OtherComp) && OtherComp->IsSimulatingPhysics())
		{
			OtherComp->AddImpulseAtLocation(NormalImpulse.GetSafeNormal(), Hit.ImpactPoint, Hit.BoneName);
		}

		GetStaticMeshComponent()->OnComponentHit.RemoveAll(this);
	}
}

void APEThrowableActor::ApplyThrowableEffect(UAbilitySystemComponent* TargetABSC)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (UPEAbilitySystemComponent* TargetGASC = Cast<UPEAbilitySystemComponent>(TargetABSC))
	{
		for (const FGameplayEffectGroupedData& Effect : HitEffects)
		{
			TargetGASC->ApplyEffectGroupedDataToSelf(Effect);
		}
	}
}
