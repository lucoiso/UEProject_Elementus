// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Actors/World/PEConsumableActor.h"
#include "GAS/System/PEAbilitySystemComponent.h"

#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"

APEConsumableActor::APEConsumableActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	ObjectMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Object Mesh"));
	ObjectVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Object VFX"));
}

void APEConsumableActor::PerformConsumption(UAbilitySystemComponent* TargetABSC,
                                            const bool bDestroyAfterConsumption = true)
{
	if (UPEAbilitySystemComponent* TargetGASC = Cast<UPEAbilitySystemComponent>(TargetABSC); ensureMsgf(
		IsValid(TargetGASC), TEXT("%s have a invalid target"), *GetName()))
	{
		if (GetLocalRole() != ROLE_Authority)
		{
			return;
		}

		if (TargetGASC->HasAllMatchingGameplayTags(RequirementsTags) || RequirementsTags.IsEmpty())
		{
			for (const FGameplayEffectGroupedData& Effect : ConsumableEffects)
			{
				TargetGASC->ApplyEffectGroupedDataToSelf(Effect);
			}

			if (bDestroyAfterConsumption)
			{
				Destroy();
			}
		}
	}
}
