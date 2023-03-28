// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "PEThrowableActor.h"
#include <Core/PEAbilityData.h>
#include <Core/PEAbilitySystemComponent.h>
#include <AbilitySystemGlobals.h>
#include <GameFramework/Character.h>
#include <Components/StaticMeshComponent.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(PEThrowableActor)

APEThrowableActor::APEThrowableActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bReplicates = true;
	bNetStartup = false;
	bNetLoadOnClient = false;
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	bOnlyRelevantToOwner = false;
	bAlwaysRelevant = false;
	AActor::SetReplicateMovement(false);
	NetUpdateFrequency = 30.f;
	NetPriority = 1.f;
	NetDormancy = ENetDormancy::DORM_Initial;

	SetRootComponent(GetStaticMeshComponent());
	SetMobility(EComponentMobility::Movable);

	GetStaticMeshComponent()->SetIsReplicated(true);
	GetStaticMeshComponent()->SetSimulatePhysics(true);
	GetStaticMeshComponent()->SetGenerateOverlapEvents(true);
	GetStaticMeshComponent()->SetNotifyRigidBodyCollision(true);
	GetStaticMeshComponent()->SetCollisionObjectType(ECC_PhysicsBody);
	GetStaticMeshComponent()->SetCollisionProfileName(TEXT("PhysicsBody"));
	GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

void APEThrowableActor::Throw(AActor* CallerActor, const FVector& Velocity)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	Caller.Reset();
	Caller = CallerActor;

	GetStaticMeshComponent()->OnComponentHit.AddDynamic(this, &APEThrowableActor::OnThrowableHit);

	GetStaticMeshComponent()->WakeAllRigidBodies();
	GetStaticMeshComponent()->AddImpulse(Velocity, NAME_None, true);

	SetNetDormancy(ENetDormancy::DORM_DormantAll);
}

void APEThrowableActor::OnThrowableHit([[maybe_unused]] UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent* OtherComp, const FVector NormalImpulse, const FHitResult& Hit)
{
	if (!IsValid(OtherActor) || OtherActor == Caller.Get())
	{
		return;
	}

	if (OtherActor->GetClass()->IsChildOf<ACharacter>())
	{
		if (ACharacter* const Character = Cast<ACharacter>(OtherActor))
		{
			constexpr float ImpulseMultiplier = 5.f;
			Character->LaunchCharacter(NormalImpulse.GetSafeNormal() * ImpulseMultiplier, false, false);
		}
	}
	else if (IsValid(OtherComp) && OtherComp->IsSimulatingPhysics() && OtherComp->Mobility == EComponentMobility::Movable)
	{		
		OtherComp->AddImpulseAtLocation(NormalImpulse.GetSafeNormal(), Hit.ImpactPoint, Hit.BoneName);
	}

	if (UAbilitySystemComponent* const TargetABSC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor))
	{
		ApplyThrowableEffect(TargetABSC);
	}

	GetStaticMeshComponent()->OnComponentHit.RemoveAll(this);
	Caller.Reset();

	SetNetDormancy(ENetDormancy::DORM_Awake);
}

void APEThrowableActor::ApplyThrowableEffect(UAbilitySystemComponent* TargetABSC)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (UPEAbilitySystemComponent* const TargetGASC = Cast<UPEAbilitySystemComponent>(TargetABSC))
	{
		for (const FGameplayEffectGroupedData& Effect : HitEffects)
		{
			TargetGASC->ApplyEffectGroupedDataToSelf(Effect);
		}
	}
}
