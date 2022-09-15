// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Actors/Interfaces/PEInteractable.h"
#include "GameFramework/Actor.h"
#include "Management/Data/PEConsumableData.h"
#include "PEConsumableActor.generated.h"

/**
 *
 */
UCLASS(Abstract, Blueprintable, Category = "Project Elementus | Classes")
class PROJECTELEMENTUS_API APEConsumableActor : public AActor, public IPEInteractable
{
	GENERATED_BODY()

public:
	explicit APEConsumableActor(const FObjectInitializer& ObjectInitializer);

	virtual void DoInteractionBehavior_Implementation(APECharacter* CharacterInteracting, const FHitResult& HitResult) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Project Elementus | Properties")
	bool bDestroyAfterConsumption;

protected:
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	void PerformConsumption(class UAbilitySystemComponent* TargetABSC);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Project Elementus | Properties")
	TObjectPtr<UPEConsumableData> ConsumableData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Project Elementus | Properties")
	TObjectPtr<UStaticMeshComponent> ObjectMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Project Elementus | Properties")
	TObjectPtr<class UNiagaraComponent> ObjectVFX;

private:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif WITH_EDITOR
};
