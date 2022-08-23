// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Actors/World/PEConsumableActor.h"

#include "AbilitySystemGlobals.h"
#include "GAS/System/PEAbilitySystemComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "Actors/Character/PECharacter.h"

APEConsumableActor::APEConsumableActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	  bDestroyAfterConsumption(true)
{
	bReplicates = false;
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	ObjectMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Object Mesh"));
	ObjectMesh->SetCollisionProfileName("Consumable");
	ObjectMesh->SetupAttachment(RootComponent);

	ObjectVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Object VFX"));
	ObjectVFX->bAutoActivate = true;
	ObjectVFX->SetupAttachment(ObjectMesh);
}

void APEConsumableActor::PerformConsumption(UAbilitySystemComponent* TargetABSC)
{
	if (UPEAbilitySystemComponent* TargetGASC = Cast<UPEAbilitySystemComponent>(TargetABSC);
		TargetGASC->HasAllMatchingGameplayTags(ConsumableData->RequirementsTags)
		|| ConsumableData->RequirementsTags.IsEmpty())
	{
		for (const FGameplayEffectGroupedData& Effect : ConsumableData->ConsumableEffects)
		{
			TargetGASC->ApplyEffectGroupedDataToSelf(Effect);
		}

		if (bDestroyAfterConsumption)
		{
			Destroy();
		}
	}
}

void APEConsumableActor::DoInteractionBehavior_Implementation(APECharacter* CharacterInteracting,
                                                              const FHitResult& HitResult)
{
	// Only call SetReplicates if has authority
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (ensureAlwaysMsgf(IsValid(CharacterInteracting->GetAbilitySystemComponent()),
	                     TEXT("%s have a invalid Ability System Component"),
	                     *CharacterInteracting->GetName()))
	{
		// Only replicates while a character is consuming
		SetReplicates(true);
		PerformConsumption(CharacterInteracting->GetAbilitySystemComponent());
		SetReplicates(false);
	}
}

#if WITH_EDITOR
void APEConsumableActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property == nullptr)
	{
		return;
	}
	
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(APEConsumableActor, ConsumableData))
	{
		// Update data with values in the given UDataAsset
		if (IsValid(ConsumableData))
		{
			!ConsumableData->ObjectMesh.IsNull()
				? ObjectMesh->SetStaticMesh(ConsumableData->ObjectMesh.LoadSynchronous())
				: ObjectMesh->SetStaticMesh(nullptr);

			!ConsumableData->ObjectVFX.IsNull()
				? ObjectVFX->SetAsset(ConsumableData->ObjectVFX.LoadSynchronous())
				: ObjectVFX->SetAsset(nullptr);
		}
	}
}
#endif WITH_EDITOR
