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
#pragma once

#include "ShaderParameterMacros.h"
#include "Shader.h"

#include "Runtime/Launch/Resources/Version.h"

// ===============================================================================================================================================
//
// FIDELITYFX CONFIGURATION
//
// ===============================================================================================================================================
#define A_CPU 1
// colliding ffx_a.ush/UE4Engine definitions
// ffx_a.ush will override these definitions
#ifdef A_STATIC
#undef A_STATIC
#endif
#ifdef A_RESTRICT
#undef A_RESTRICT
#endif
#include "../Shaders/Private/ffx_a.ush"
#include "../Shaders/Private/ffx_fsr1.ush"

// ===============================================================================================================================================
//
// SHADER DEFINITIONS
//
// ===============================================================================================================================================

//
// SHADER PASS PARAMETERS
//
enum class EFSR_OutputDevice // should match PostProcessFFX_Common.ush
{
	sRGB = 0,
	Linear,
	PQ,
	scRGB,
	MAX
};
enum class EFSR_OutputDeviceMaxNits // matching UE4's scRGB/PQ-1000Nits and scRGB/PQ-2000Nits
{
	EFSR_1000Nits,
	EFSR_2000Nits,
	MAX
};

// permutation domains
class FFSR_UseFP16Dim : SHADER_PERMUTATION_BOOL("ENABLE_FP16");
class FFSR_OutputDeviceDim : SHADER_PERMUTATION_ENUM_CLASS("FSR_OUTPUTDEVICE", EFSR_OutputDevice);
class FFSR_OutputDeviceNitsDim : SHADER_PERMUTATION_ENUM_CLASS("MAX_ODT_NITS_ENUM", EFSR_OutputDeviceMaxNits);
class FFSR_OutputDeviceConversion : SHADER_PERMUTATION_BOOL("CONVERT_TO_OUTPUT_DEVICE");
class FFSR_PQDitherDim : SHADER_PERMUTATION_BOOL("ENABLE_PQ_DITHER");
class FFSR_GrainDim : SHADER_PERMUTATION_BOOL("USE_GRAIN_INTENSITY");
class FRCAS_DenoiseDim : SHADER_PERMUTATION_BOOL("USE_RCAS_DENOISE");

//
// SHADER PASS PARAMETERS
//
BEGIN_SHADER_PARAMETER_STRUCT(FFSRPassParameters_Grain, )
#if ENGINE_MAJOR_VERSION >= 5
	SHADER_PARAMETER(FVector4f, GrainRandomFull)
	SHADER_PARAMETER(FVector4f, GrainScaleBiasJitter)
#else
	SHADER_PARAMETER(FVector4, GrainRandomFull)
	SHADER_PARAMETER(FVector4, GrainScaleBiasJitter)
#endif
END_SHADER_PARAMETER_STRUCT()

BEGIN_SHADER_PARAMETER_STRUCT(FFSRPassParameters, )
	SHADER_PARAMETER(FUintVector4, Const0)
	SHADER_PARAMETER(FUintVector4, Const1)
	SHADER_PARAMETER(FUintVector4, Const2)
	SHADER_PARAMETER(FUintVector4, Const3)
	SHADER_PARAMETER_STRUCT_INCLUDE(FFSRPassParameters_Grain, FilmGrain)

#if ENGINE_MAJOR_VERSION >= 5
	SHADER_PARAMETER(FVector2f, VPColor_ExtentInverse)
	SHADER_PARAMETER(FVector2f, VPColor_ViewportMin)
#else
	SHADER_PARAMETER(FVector2D, VPColor_ExtentInverse)
	SHADER_PARAMETER(FVector2D, VPColor_ViewportMin)
#endif

	SHADER_PARAMETER_SAMPLER(SamplerState, samLinearClamp)
	SHADER_PARAMETER_RDG_TEXTURE(Texture2D, InputTexture)
END_SHADER_PARAMETER_STRUCT()

BEGIN_SHADER_PARAMETER_STRUCT(FRCASPassParameters, )
	SHADER_PARAMETER_RDG_TEXTURE(Texture2D, InputTexture)
	SHADER_PARAMETER(FUintVector4, Const0)
	SHADER_PARAMETER_STRUCT_INCLUDE(FFSRPassParameters_Grain, FilmGrain)

#if ENGINE_MAJOR_VERSION >= 5
	SHADER_PARAMETER(FVector2f, VPColor_ExtentInverse)
#else
	SHADER_PARAMETER(FVector2D, VPColor_ExtentInverse)
#endif
END_SHADER_PARAMETER_STRUCT()