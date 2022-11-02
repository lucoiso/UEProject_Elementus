// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Actors/Character/PEHUD.h"
#include "Blueprint/UserWidget.h"

APEHUD::APEHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	static ConstructorHelpers::FClassFinder<UUserWidget> UserHUD_ClassRef(TEXT("/Game/Main/Blueprints/Widgets/WB_HUD"));
	if (UserHUD_ClassRef.Succeeded())
	{
		HUDClass = UserHUD_ClassRef.Class;
	}
}

void APEHUD::BeginPlay()
{
	// Initialize a Blueprint Widget as HUD if Player Controller is valid
	if (ensureAlwaysMsgf(IsValid(GetOwningPlayerController()), TEXT("%s have a invalid Controller"), *GetName()))
	{
		HUDHandle = CreateWidget(GetOwningPlayerController(), HUDClass, TEXT("Main HUD"));
		HUDHandle->AddToPlayerScreen();
	}

	Super::BeginPlay();
}
