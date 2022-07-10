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
#include "FSRSpatialUpscaling.h"
#include "LogFSR.h"

#include "CoreMinimal.h"
#include "Interfaces/IPluginManager.h"

IMPLEMENT_MODULE(FFSRSpatialUpscalingModule, FSRSpatialUpscaling)

#define LOCTEXT_NAMESPACE "FSR"

DEFINE_LOG_CATEGORY(LogFSR);

void FFSRSpatialUpscalingModule::StartupModule()
{
	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("FSR"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/Plugin/FSR"), PluginShaderDir);

	UE_LOG(LogFSR, Log, TEXT("FSR Spatial Upscaling Module Started"));
}

void FFSRSpatialUpscalingModule::ShutdownModule()
{
	UE_LOG(LogFSR, Log, TEXT("FSR Spatial Upscaling Module Shutdown"));
}

#undef LOCTEXT_NAMESPACE