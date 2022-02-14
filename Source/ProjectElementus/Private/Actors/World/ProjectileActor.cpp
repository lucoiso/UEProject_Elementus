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
	APECharacterBase* Character = Cast<APECharacterBase>(OtherActor);

	if (OtherComp->IsSimulatingPhysics() && !IsValid(Character))
	{
		OtherComp->AddImpulseAtLocation(ImpulseVelocity, Hit.ImpactPoint, Hit.BoneName);
	}
	else if (IsValid(Character))
	{
		Character->LaunchCharacter(ImpulseVelocity, true, true);

		UAbilitySystemComponent* AbilitySystem = Character->GetAbilitySystemComponent();
		if (IsValid(AbilitySystem))
		{
			ApplyProjectileEffect(AbilitySystem);
		}
	}

	Destroy();
}

void AProjectileActor::ApplyProjectileEffect_Implementation(UAbilitySystemComponent* TargetComp)
{
	if (GetLocalRole() != ROLE_Authority || !IsValid(TargetComp))
	{
		return;
	}
	for (FGameplayEffectSpecHandle SpecHandle : DamageEffectSpecHandles)
	{
		TargetComp->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

bool AProjectileActor::ApplyProjectileEffect_Validate(UAbilitySystemComponent* TargetComp)
{
	return true;
}
