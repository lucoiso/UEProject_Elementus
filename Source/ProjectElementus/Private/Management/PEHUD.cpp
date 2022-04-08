// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Management/PEHUD.h"
#include "Blueprint/UserWidget.h"

APEHUD::APEHUD(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	static const ConstructorHelpers::FClassFinder<UUserWidget> UserHUD_ClassRef(
		TEXT("/Game/Main/Blueprints/Widgets/BP_ScreenInformations"));
#if __cplusplus > 201402L // Check if C++ > C++14
	if constexpr (&UserHUD_ClassRef.Class != nullptr)
#else
	if (&UserHUD_ClassRef.Class != nullptr)
#endif
	{
		HUDClass = UserHUD_ClassRef.Class;
	}
}

void APEHUD::BeginPlay()
{
	if (ensureMsgf(IsValid(GetOwningPlayerController()), TEXT("%s have a invalid Controller"), *GetActorLabel()))
	{
		HUDHandle = CreateWidget(GetOwningPlayerController(), HUDClass, FName("Main HUD"));
		HUDHandle->AddToPlayerScreen();
	}

	Super::BeginPlay();
}

void APEHUD::HideHUD()
{
	if (HUDHandle.IsValid())
	{
		HUDHandle->RemoveFromParent();
		HUDHandle.Reset();
	}
}