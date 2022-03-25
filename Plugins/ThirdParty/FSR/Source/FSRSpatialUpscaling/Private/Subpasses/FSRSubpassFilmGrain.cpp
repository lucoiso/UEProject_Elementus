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
#include "FSRSubpassFilmGrain.h"

static TAutoConsoleVariable<int32> CVarFSRPostFSRFilmGrain(
	TEXT("r.FidelityFX.FSR.Post.FilmGrain"),
	1,
	TEXT("Applies FilmGrain after FidelityFX Super Resolution on the upscaled image instead of during tonemapping (in low-res) to avoid upscaled film grain effect for a more realistic look"),
	ECVF_RenderThreadSafe);

static FFSRPassParameters_Grain GetFilmGrainParameters(const FViewInfo& View, const float GrainIntensity, const float GrainJitter)
{
	// this section copy-pasted from PostProcessTonemap.cpp (with modifications) to re-generate Grain Intensity parameters ----------------
	FVector3f GrainRandomFullValue;
	{
		uint8 FrameIndexMod8 = 0;
		if (View.Family)
		{
			FrameIndexMod8 = View.Family->FrameNumber & 0x7;
		}
		
		GrainRandomFromFrame(&GrainRandomFullValue, FrameIndexMod8);
	}

	FVector GrainScaleBiasJitter;
	auto fnGrainPostSettings = [](FVector* RESTRICT const Constant, const FPostProcessSettings* RESTRICT const Settings, const float GrainIntensity, const float GrainJitter)
	{
		Constant->X = GrainIntensity;
		Constant->Y = 1.0f + (-0.5f * GrainIntensity);
		Constant->Z = GrainJitter;
	};
	fnGrainPostSettings(&GrainScaleBiasJitter, &View.FinalPostProcessSettings, GrainIntensity, GrainJitter);
	// this section copy-pasted from PostProcessTonemap.cpp (with modifications) to re-generate Grain Intensity parameters ----------------

	FFSRPassParameters_Grain Parameters;
	Parameters.GrainRandomFull = FVector4f(GrainRandomFullValue.X, GrainRandomFullValue.Y, 0, 0);
	Parameters.GrainScaleBiasJitter = FVector4f(GrainScaleBiasJitter, 0);
	return Parameters;
}

void FFSRSubpassFilmGrain::ParseEnvironment(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FInputs& PassInputs)
{
	Data->bUSE_GRAIN_INTENSITY = CVarFSRPostFSRFilmGrain->GetInt() > 0 && Data->PostProcess_GrainIntensity > 0.0f;
	Data->FilmGrainParams = GetFilmGrainParameters(View, Data->PostProcess_GrainIntensity, Data->PostProcess_GrainJitter);
}