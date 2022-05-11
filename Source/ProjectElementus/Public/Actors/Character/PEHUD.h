// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PEHUD.generated.h"

/**
 *
 */
UCLASS(Blueprintable, NotPlaceable, Category = "Project Elementus | Classes")
class PROJECTELEMENTUS_API APEHUD final : public AHUD
{
	GENERATED_BODY()

public:
	explicit APEHUD(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	void HideHUD();

protected:
	/* A Blueprint Widget class to use as HUD */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Project Elementus | Properties")
	TSubclassOf<UUserWidget> HUDClass;

private:
	TWeakObjectPtr<UUserWidget> HUDHandle;
};
