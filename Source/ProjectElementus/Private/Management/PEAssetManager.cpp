// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Management/PEAssetManager.h"
#include "GAS/System/PEAbilitySystemGlobals.h"

void UPEAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	UPEAbilitySystemGlobals::Get().InitGlobalData();
}