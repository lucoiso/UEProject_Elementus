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
#include "FSRSpatialUpscaler.h"

#include "Subpasses/FSRSubpassFirst.h"
#include "Subpasses/FSRSubpassEASU.h"
#include "Subpasses/FSRSubpassRCAS.h"
#include "Subpasses/FSRSubpassFilmGrain.h"
#include "Subpasses/FSRSubpassHDR.h"
#include "Subpasses/FSRSubpassChromaticAberration.h"
#include "Subpasses/FSRSubpassLast.h"

#define EXECUTE_STEP(step) \
	for (FFSRSubpass* Subpass : FSRSubpasses) \
	{ \
		Subpass->step(GraphBuilder, View, PassInputs); \
	}

DECLARE_GPU_STAT(FidelityFXSuperResolutionPass)

FFSRSpatialUpscaler::FFSRSpatialUpscaler(EFSRMode InMode, TArray<TSharedPtr<FFSRData>> InViewData)
	: Mode(InMode)
	, ViewData(InViewData)
{
	if (Mode != EFSRMode::None)
	{
		// subpasses will run in the order in which they are registered
		 
		// ensure this subpass always runs first
		RegisterSubpass<FFSRSubpassFirst>();

		RegisterSubpass<FFSRSubpassHDR>();
		RegisterSubpass<FFSRSubpassEASU>();
		RegisterSubpass<FFSRSubpassRCAS>();
		RegisterSubpass<FFSRSubpassFilmGrain>();
		RegisterSubpass<FFSRSubpassChromaticAberration>();

		// ensure this subpass always runs last.
		RegisterSubpass<FFSRSubpassLast>();
	}
}

ISpatialUpscaler* FFSRSpatialUpscaler::Fork_GameThread(const class FSceneViewFamily& ViewFamily) const
{
	// the object we return here will get deleted by UE4 when the scene view tears down, so we need to instantiate a new one every frame.
	return new FFSRSpatialUpscaler(Mode, ViewData);
}

FScreenPassTexture FFSRSpatialUpscaler::AddPasses(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FInputs& PassInputs) const
{
	RDG_GPU_STAT_SCOPE(GraphBuilder, FidelityFXSuperResolutionPass);
	check(PassInputs.SceneColor.IsValid());

	TSharedPtr<FFSRData> Data = GetDataForView(View);
	for (FFSRSubpass* Subpass : FSRSubpasses)
	{
		Subpass->SetData(Data.Get());
	}

	if (!Data->bInitialized)
	{
		EXECUTE_STEP(ParseEnvironment);
		EXECUTE_STEP(CreateResources);
	}

	if (Mode == EFSRMode::UpscalingOnly || Mode == EFSRMode::Combined)
	{
		EXECUTE_STEP(Upscale);
	}

	if (Mode == EFSRMode::PostProcessingOnly || Mode == EFSRMode::Combined)
	{
		EXECUTE_STEP(PostProcess);
	}

	FScreenPassTexture FinalOutput = Data->FinalOutput;
	return MoveTemp(FinalOutput);
}

TSharedPtr<FFSRData> FFSRSpatialUpscaler::GetDataForView(const FViewInfo& View) const
{
	for (int i = 0; i < View.Family->Views.Num(); i++)
	{
		if (View.Family->Views[i] == &View)
		{
			return ViewData[i];
		}
	}
	return nullptr;
}