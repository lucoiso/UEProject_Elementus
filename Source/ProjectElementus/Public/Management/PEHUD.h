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
UCLASS(Blueprintable, NotPlaceable, Category = "Custom Classes | Management")
class PROJECTELEMENTUS_API APEHUD : public AHUD
{
	GENERATED_BODY()

public:
	APEHUD(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	void HideHUD();

protected:
	/* A Blueprint Widget class to use as HUD */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Custom Properties | Management")
		TSubclassOf<UUserWidget> HUDClass;

private:
	TWeakObjectPtr<UUserWidget> HUDHandle;
};
