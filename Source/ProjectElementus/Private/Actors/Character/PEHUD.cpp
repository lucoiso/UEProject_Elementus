// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Actors/Character/PEHUD.h"
#include "Management/PEProjectSettings.h"
#include <Blueprint/UserWidget.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(PEHUD)

APEHUD::APEHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void APEHUD::BeginPlay()
{
	Super::BeginPlay();

	// Initialize a Blueprint Widget as HUD if Player Controller is valid
	if (ensureAlwaysMsgf(IsValid(GetOwningPlayerController()), TEXT("%s have a invalid Controller"), *GetName()))
	{
		const UPEProjectSettings* const ProjectSettings = GetDefault<UPEProjectSettings>();
		const TSubclassOf<UUserWidget> UMGHUDClass = ProjectSettings->HUDClass.IsNull() ? nullptr : ProjectSettings->HUDClass.LoadSynchronous();

		if (!IsValid(UMGHUDClass))
		{
			UE_LOG(LogTemp, Error, TEXT("%s - Missing setting: HUD Class"), *FString(__func__));
			return;
		}

		if (HUDHandle = CreateWidget(GetOwningPlayerController(), UMGHUDClass, TEXT("Main HUD"));
			HUDHandle.IsValid())
		{
			HUDHandle->AddToPlayerScreen();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("%s - Failed to initialize UMG HUD"), *FString(__func__));
		}
	}
}
