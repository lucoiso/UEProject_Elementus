// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "Actors/Character/PECharacterBase.h"
#include "PECharacter.generated.h"

/**
 *
 */
UCLASS(config = Game, Category = "Custom Classes | Player")
class APECharacter : public APECharacterBase
{
	GENERATED_BODY()

public:
	APECharacter(const FObjectInitializer& ObjectInitializer);

private:
	bool bInputBound = false;

	void BindASCInput();

protected:
	virtual void PossessedBy(AController* InputController) override;

	virtual void OnRep_PlayerState() override;

	virtual void OnRep_Controller() override;

private:
	virtual void Landed(const FHitResult& Hit) final override;
};
