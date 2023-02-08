// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Management/PEAssetManager.h"
#include <AbilitySystemGlobals.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(PEAssetManager)

void UPEAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	// Should be called once as part of project setup to load global data tables and tags - From AbilitySystemGlobals.h
	UAbilitySystemGlobals::Get().InitGlobalData();
}
