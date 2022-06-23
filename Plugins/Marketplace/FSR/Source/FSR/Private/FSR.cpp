//------------------------------------------------------------------------------
// FidelityFX Super Resolution UE4 Plugin
//
// Copyright (c) 2021 Advanced Micro Devices, Inc. All rights reserved.
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//------------------------------------------------------------------------------
#include "FSR.h"
#include "FSRViewExtension.h"

#include "CoreMinimal.h"

#include "Runtime/Launch/Resources/Version.h"
#include "Engine/Engine.h"
#include "Engine/Texture2D.h"

static_assert((ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION == 27 && ENGINE_PATCH_VERSION >= 1) || (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION > 27) || ENGINE_MAJOR_VERSION >= 5, "FSR plugin requires UE4.27.1 or greater.");

IMPLEMENT_MODULE(FFSRModule, FSR)

// DUPLICATED FROM UNREALENGINE.CPP
/** Special function that loads an engine texture and adds it to root set in cooked builds.
  * This is to prevent the textures from being added to GC clusters (root set objects are not clustered)
	* because otherwise since they're always going to be referenced by UUnrealEngine object which will
	* prevent the clusters from being GC'd. */
	template <typename TextureType>
static void LoadEngineTexture(TextureType*& InOutTexture, const TCHAR* InName)
{
	if (!InOutTexture)
	{
		InOutTexture = LoadObject<TextureType>(nullptr, InName, nullptr, LOAD_None, nullptr);
	}
	if (FPlatformProperties::RequiresCookedData() && InOutTexture)
	{
		InOutTexture->AddToRoot();
	}
}
// END OF DUPLICATED FROM UNREALENGINE.CPP

void FFSRModule::StartupModule()
{
	FSRViewExtension = FSceneViewExtensions::NewExtension<FFSRViewExtension>();

	if (GEngine->BlueNoiseTexture == nullptr)
	{
		GEngine->LoadBlueNoiseTexture();
	}
}

void FFSRModule::ShutdownModule()
{
	// this is a smart pointer.  setting it to null is the correct way to release its memory.
	FSRViewExtension = nullptr;
}