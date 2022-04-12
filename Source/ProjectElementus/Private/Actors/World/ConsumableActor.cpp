// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Actors/World/ConsumableActor.h"
#include "Components/StaticMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "NiagaraComponent.h"

AConsumableActor::AConsumableActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	ObjectMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Object Mesh"));
	ObjectVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Object VFX"));
}

void AConsumableActor::PerformConsumption_Implementation(UAbilitySystemComponent* TargetABSC, const bool bDestroyAfterConsumption = true)
{
	if (ensureMsgf(IsValid(TargetABSC), TEXT("%s have a invalid target"), *GetName()))
	{
		if (!TargetABSC->IsOwnerActorAuthoritative())
		{
			return;
		}

		if (TargetABSC->HasAllMatchingGameplayTags(RequirementsTags) || RequirementsTags.IsEmpty())
		{
			TargetABSC->ApplyGameplayEffectToSelf(ObjectEffectClass.GetDefaultObject(), 1.f,
				TargetABSC->MakeEffectContext());

			if (bDestroyAfterConsumption)
			{
				Destroy();
			}
		}
	}
}

bool AConsumableActor::PerformConsumption_Validate(UAbilitySystemComponent* TargetABSC, const bool bDestroyAfterConsumption = true)
{
	return true;
}