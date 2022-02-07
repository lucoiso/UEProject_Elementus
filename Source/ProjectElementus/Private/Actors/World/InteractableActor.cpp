#include "Actors/World/InteractableActor.h"
#include "AbilitySystemComponent.h"

AInteractableActor::AInteractableActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void AInteractableActor::PerformInteraction(UAbilitySystemComponent* TargetABSC)
{
	if (TargetABSC->HasAllMatchingGameplayTags(RequirementsTags))
	{
		DoInteractionBehavior();
	}
}
