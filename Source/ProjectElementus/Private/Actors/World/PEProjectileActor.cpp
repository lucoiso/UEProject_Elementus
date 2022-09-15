// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Actors/World/PEProjectileActor.h"
#include "Actors/Character/PECharacter.h"
#include "GAS/System/PEAbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

APEProjectileActor::APEProjectileActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bReplicates = true;
	bAlwaysRelevant = true;
	AActor::SetReplicateMovement(true);

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Component"));
	CollisionComponent->InitSphereRadius(12.5f);
	CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));

	RootComponent = CollisionComponent;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement Component"));
	ProjectileMovement->SetUpdatedComponent(CollisionComponent);

	InitialLifeSpan = 3.f;

	ProjectileMovement->InitialSpeed = 2500.f;
	ProjectileMovement->MaxSpeed = 3000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->Bounciness = 0.25f;
	ProjectileMovement->ProjectileGravityScale = 0.025f;
}

void APEProjectileActor::BeginPlay()
{
	Super::BeginPlay();
	CollisionComponent->OnComponentHit.AddDynamic(this, &APEProjectileActor::OnProjectileHit);
}

void APEProjectileActor::FireInDirection(const FVector Direction) const
{
	ProjectileMovement->Velocity = ProjectileMovement->InitialSpeed * Direction;
}

void APEProjectileActor::OnProjectileHit_Implementation(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	const FVector ImpulseVelocity = ProjectileMovement->Velocity * (ImpulseMultiplier / 10.f);

	if (IsValid(OtherActor) && OtherActor->GetClass()->IsChildOf<APECharacter>())
	{
		if (APECharacter* const Character = Cast<APECharacter>(OtherActor))
		{
			Character->LaunchCharacter(ImpulseVelocity, true, true);

			if (UPEAbilitySystemComponent* const TargetGASC = Cast<UPEAbilitySystemComponent>(Character->GetAbilitySystemComponent());
				ensureAlwaysMsgf(IsValid(TargetGASC), TEXT("%s have a invalid target"), *GetName()))
			{
				ApplyProjectileEffect(TargetGASC);
			}
		}
	}
	else if (IsValid(OtherComp) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(ImpulseVelocity, Hit.ImpactPoint, Hit.BoneName);
	}

	Destroy();
}

void APEProjectileActor::ApplyProjectileEffect(UAbilitySystemComponent* TargetABSC)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (UPEAbilitySystemComponent* const TargetGASC = Cast<UPEAbilitySystemComponent>(TargetABSC))
	{
		for (const FGameplayEffectGroupedData& Effect : ProjectileEffects)
		{
			TargetGASC->ApplyEffectGroupedDataToSelf(Effect);
		}
	}
}
