// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "ThrowableActor.h"
#include "Components/StaticMeshComponent.h"
#include "Actors/Character/PECharacterBase.h"
#include "AbilitySystemComponent.h"

AThrowableActor::AThrowableActor(const FObjectInitializer& ObjectInitializer)
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

void AThrowableActor::ThrowSetup(AActor* Caller)
{	
	CallerActor.Reset();
	CallerActor = Caller;
	
	if (!GetStaticMeshComponent()->OnComponentHit.IsBound())
	{
		GetStaticMeshComponent()->OnComponentHit.AddDynamic(this, &AThrowableActor::OnThrowableHit);
	}
}

void AThrowableActor::OnThrowableHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{		
	GetStaticMeshComponent()->OnComponentHit.RemoveAll(this);
	GetStaticMeshComponent()->AddForceAtLocation(NormalImpulse, Hit.ImpactPoint);
	
	if (IsValid(OtherActor) && OtherActor->GetClass()->IsChildOf<APECharacterBase>() && OtherActor != CallerActor.Get())
	{
		APECharacterBase* Player = Cast<APECharacterBase>(OtherActor);

		if (ensureMsgf(IsValid(Player), TEXT("%s have a invalid Player"), *GetActorLabel()))
		{
			constexpr float ImpulseMultiplier = 5.f;
			
			Player->LaunchCharacter(NormalImpulse.GetSafeNormal() * ImpulseMultiplier, false, false);
			GetStaticMeshComponent()->AddImpulse(NormalImpulse.GetSafeNormal() * (ImpulseMultiplier / -2.f));
			
			if (ensureMsgf(IsValid(Player->GetAbilitySystemComponent()), TEXT("%s have a invalid Ability System Component"), *Player->GetActorLabel()))
			{
				ApplyThrowableEffect(Player->GetAbilitySystemComponent());
			}			
		}
	}	
}

void AThrowableActor::ApplyThrowableEffect_Implementation(UAbilitySystemComponent* TargetComp)
{
	if (ensureMsgf(IsValid(TargetComp), TEXT("%s have a invalid target"), *GetActorLabel()))
	{
		if (GetLocalRole() != ROLE_Authority)
		{
			return;
		}

		for (const TSubclassOf<UGameplayEffect>& Effect : HitEffects)
		{
			TargetComp->ApplyGameplayEffectToSelf(Effect.GetDefaultObject(),
				1.f, TargetComp->MakeEffectContext());
		}
	}
}

bool AThrowableActor::ApplyThrowableEffect_Validate(UAbilitySystemComponent* TargetComp)
{
	return true;
}
