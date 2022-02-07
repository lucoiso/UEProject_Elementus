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

void AConsumableActor::PerformConsumption_Implementation(UAbilitySystemComponent* TargetABSC)
{
	if (ensureMsgf(IsValid(TargetABSC), TEXT("%s have a invalid target"), *GetActorLabel()))
	{
		if (TargetABSC->HasAllMatchingGameplayTags(RequirementsTags) && TargetABSC->IsOwnerActorAuthoritative())
		{
			TargetABSC->ApplyGameplayEffectToSelf(ObjectEffectClass.GetDefaultObject(), 1.f,
			                                      TargetABSC->MakeEffectContext());
		}

		Destroy();
	}
}

bool AConsumableActor::PerformConsumption_Validate(UAbilitySystemComponent* TargetABSC)
{
	return true;
}
