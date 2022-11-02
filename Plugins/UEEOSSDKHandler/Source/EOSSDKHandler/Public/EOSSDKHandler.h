// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEEOSSDKHandler

#pragma once

#include "Runtime/Launch/Resources/Version.h"

#if ENGINE_MAJOR_VERSION >= 5
#include "Modules/ModuleInterface.h"
#else
#include "Modules/ModuleManager.h"
#endif

/**
 *
 */

class FEOSSDKHandlerModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};