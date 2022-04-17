// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "PEAssetManager.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI, Category = "Custom Classes | Management")
class UPEAssetManager : public UAssetManager
{
	GENERATED_BODY()
		
public:
	virtual void StartInitialLoading() override;	
};
