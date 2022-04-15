// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

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
	if (ensureMsgf(IsValid(TargetABSC), TEXT("%s have a invalid target"), *GetName()))
	{		
		if (TargetABSC->HasAllMatchingGameplayTags(RequirementsTags) || RequirementsTags.IsEmpty())
		{
			DoInteractionBehavior();
		}
	}
}