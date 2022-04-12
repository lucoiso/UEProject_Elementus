// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Actors/World/ProjectileActor.h"
#include "Actors/Character/PECharacterBase.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "AbilitySystemComponent.h"

AProjectileActor::AProjectileActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReplicates = true;
	bAlwaysRelevant = true;
	SetReplicateMovement(true);

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

void AProjectileActor::BeginPlay()
{
	Super::BeginPlay();

	CollisionComponent->OnComponentHit.AddDynamic(this, &AProjectileActor::OnProjectileHit);
}

void AProjectileActor::FireInDirection(const FVector Direction)
{
	ProjectileMovement->Velocity = ProjectileMovement->InitialSpeed * Direction;
}

void AProjectileActor::OnProjectileHit_Implementation(UPrimitiveComponent* HitComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse,
	const FHitResult& Hit)
{
	const FVector ImpulseVelocity = ProjectileMovement->Velocity * (ImpulseMultiplier / 10.f);

	if (IsValid(OtherActor) && OtherActor->GetClass()->IsChildOf<APECharacterBase>())
	{
		APECharacterBase* Character = Cast<APECharacterBase>(OtherActor);
		if (ensureMsgf(IsValid(Character), TEXT("%s have a invalid Character"), *GetName()))
		{
			Character->LaunchCharacter(ImpulseVelocity, true, true);
			ApplyProjectileEffect(Character->GetAbilitySystemComponent());
		}
	}
	else if (IsValid(OtherComp) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(ImpulseVelocity, Hit.ImpactPoint, Hit.BoneName);
	}

	Destroy();
}

void AProjectileActor::ApplyProjectileEffect_Implementation(UAbilitySystemComponent* TargetComp)
{
	if (ensureMsgf(IsValid(TargetComp), TEXT("%s have a invalid target"), *GetName()))
	{
		if (GetLocalRole() != ROLE_Authority)
		{
			return;
		}

		for (const FGameplayEffectSpecHandle& SpecHandle : DamageEffectSpecHandles)
		{
			TargetComp->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}

bool AProjectileActor::ApplyProjectileEffect_Validate(UAbilitySystemComponent* TargetComp)
{
	return true;
}