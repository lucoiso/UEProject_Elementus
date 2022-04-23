// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "PEThrowableActor.h"
#include "Components/StaticMeshComponent.h"
#include "Actors/Character/PECharacterBase.h"
#include "GAS/System/PEAbilitySystemComponent.h"

APEThrowableActor::APEThrowableActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SetRootComponent(GetStaticMeshComponent());
	GetStaticMeshComponent()->SetSimulatePhysics(true);
	GetStaticMeshComponent()->SetGenerateOverlapEvents(true);
	GetStaticMeshComponent()->SetNotifyRigidBodyCollision(true);
	GetStaticMeshComponent()->SetCollisionObjectType(ECC_PhysicsBody);
	GetStaticMeshComponent()->SetCollisionProfileName(TEXT("PhysicsBody"));
	GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	SetMobility(EComponentMobility::Movable);

	bReplicates = true;
	GetStaticMeshComponent()->SetIsReplicated(true);
}

void APEThrowableActor::ThrowSetup(AActor* Caller)
{
	CallerActor.Reset();
	CallerActor = Caller;

	GetStaticMeshComponent()->OnComponentHit.AddDynamic(this, &APEThrowableActor::OnThrowableHit);
}

void APEThrowableActor::OnThrowableHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (IsValid(OtherActor) && OtherActor != CallerActor.Get())
	{
		if (OtherActor->GetClass()->IsChildOf<APECharacterBase>())
		{
			APECharacterBase* Player = Cast<APECharacterBase>(OtherActor);

			if (ensureMsgf(IsValid(Player), TEXT("%s have a invalid Player"), *GetName()))
			{
				constexpr float ImpulseMultiplier = 5.f;

				Player->LaunchCharacter(NormalImpulse.GetSafeNormal() * ImpulseMultiplier, false, false);

				if (ensureMsgf(IsValid(Player->GetAbilitySystemComponent()), TEXT("%s have a invalid Ability System Component"), *Player->GetName()))
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
	UPEAbilitySystemComponent* TargetGASC = Cast<UPEAbilitySystemComponent>(TargetABSC);
	if (ensureMsgf(IsValid(TargetGASC), TEXT("%s have a invalid target"), *GetName()))
	{
		if (GetLocalRole() != ROLE_Authority)
		{
			return;
		}

		for (const FGameplayEffectGroupedData& Effect : HitEffects)
		{
			TargetGASC->ApplyEffectGroupedDataToSelf(Effect);
		}
	}
}