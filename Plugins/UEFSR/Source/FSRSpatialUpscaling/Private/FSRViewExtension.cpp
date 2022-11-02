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
#include "FSRViewExtension.h"
#include "FSRSpatialUpscaler.h"

static TAutoConsoleVariable<int32> CVarEnableFSR(
	TEXT("r.FidelityFX.FSR.Enabled"),
	1,
	TEXT("Enable FidelityFX Super Resolution for Primary Upscale"),
	ECVF_RenderThreadSafe);

inline static bool IsChromaticAberrationPassEnabled()
{
	static TConsoleVariableData<int32>* CVarPostFSRColorFringe = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.FidelityFX.FSR.Post.ExperimentalChromaticAberration"));
	return (CVarPostFSRColorFringe->GetValueOnAnyThread() > 0);
}

inline static bool IsFilmGrainPassEnabled()
{
	static TConsoleVariableData<int32>* CVarFSRPostFSRFilmGrain = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.FidelityFX.FSR.Post.FilmGrain"));
	return (CVarFSRPostFSRFilmGrain->GetValueOnAnyThread() > 0);
}

static bool IsEASUTheLastPass()
{
	static IConsoleVariable* CVarFSRRCASEnabled = IConsoleManager::Get().FindConsoleVariable(TEXT("r.FidelityFX.FSR.RCAS.Enabled"));
	const bool IsRCasEnabled = (CVarFSRRCASEnabled->GetInt() > 0);
	return !(IsRCasEnabled || IsChromaticAberrationPassEnabled());
}

void FFSRViewExtension::BeginRenderViewFamily(FSceneViewFamily& InViewFamily)
{
	// the object(s) we assign here will get deleted by UE4 when the scene view tears down, so we need to instantiate a new one every frame.
	if (InViewFamily.GetFeatureLevel() >= ERHIFeatureLevel::SM5 && CVarEnableFSR.GetValueOnAnyThread() > 0)
	{
#if WITH_EDITOR
		if (GIsEditor)
		{
			// if we are in the full Editor, don't let the DPI-upscaler interfere with FSR
			static auto CVarEnableEditorScreenPercentageOverride = IConsoleManager::Get().FindConsoleVariable(TEXT("Editor.OverrideDPIBasedEditorViewportScaling"));

			if (CVarEnableEditorScreenPercentageOverride != nullptr)
			{
				CVarEnableEditorScreenPercentageOverride->Set(1);				
			}
		}
#endif

		TArray<TSharedPtr<FFSRData>> ViewData;

		bool IsTemporalUpscalingRequested = false;
		for (int i = 0; i < InViewFamily.Views.Num(); i++)
		{
			const FSceneView* InView = InViewFamily.Views[i];
			if (ensure(InView))
			{
				// if any view is using temporal upscaling, use the Combined upscaling mode.
				IsTemporalUpscalingRequested |= (InView->PrimaryScreenPercentageMethod == EPrimaryScreenPercentageMethod::TemporalUpscale);

				// TSharedPtr will clean up this allocation
				FFSRData* Data = new FFSRData();
				
#if ENGINE_MAJOR_VERSION >= 5
				Data->PostProcess_GrainIntensity = InView->FinalPostProcessSettings.GrainIntensity_DEPRECATED;
				Data->PostProcess_GrainJitter = InView->FinalPostProcessSettings.GrainJitter_DEPRECATED;
#else
				Data->PostProcess_GrainIntensity = InView->FinalPostProcessSettings.GrainIntensity;
				Data->PostProcess_GrainJitter = InView->FinalPostProcessSettings.GrainJitter;
#endif
				Data->PostProcess_SceneFringeIntensity = InView->FinalPostProcessSettings.SceneFringeIntensity;
				Data->PostProcess_ChromaticAberrationStartOffset = InView->FinalPostProcessSettings.ChromaticAberrationStartOffset;
				ViewData.Add(TSharedPtr<FFSRData>(Data));
			}
		}
		
		if (!IsTemporalUpscalingRequested)
		{
			InViewFamily.SetPrimarySpatialUpscalerInterface(new FFSRSpatialUpscaler(EFSRMode::UpscalingOnly, ViewData));
			if (!IsEASUTheLastPass())
			{
				InViewFamily.SetSecondarySpatialUpscalerInterface(new FFSRSpatialUpscaler(EFSRMode::PostProcessingOnly, ViewData));
			}
		}
		else
		{
			InViewFamily.SetSecondarySpatialUpscalerInterface(new FFSRSpatialUpscaler(EFSRMode::Combined, ViewData));
		}
	}
}

void FFSRViewExtension::PreRenderView_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneView& InView)
{
	if (CVarEnableFSR.GetValueOnAnyThread() > 0)
	{
		if (IsFilmGrainPassEnabled())
		{
			// setting these values to 0 completely disables the shader permutations in PostProcessTonemap.  it doesn't just run it with no visible result.
#if ENGINE_MAJOR_VERSION >= 5
			InView.FinalPostProcessSettings.GrainIntensity_DEPRECATED = 0;
			InView.FinalPostProcessSettings.GrainJitter_DEPRECATED = 0;
#else
			InView.FinalPostProcessSettings.GrainIntensity = 0;
			InView.FinalPostProcessSettings.GrainJitter = 0;
#endif
		}

		if (IsChromaticAberrationPassEnabled())
		{
			// setting these values to 0 completely disables the shader permutations in PostProcessTonemap.  it doesn't just run it with no visible result.
			InView.FinalPostProcessSettings.SceneFringeIntensity = 0;
			InView.FinalPostProcessSettings.ChromaticAberrationStartOffset = 0;
		}
	}
}