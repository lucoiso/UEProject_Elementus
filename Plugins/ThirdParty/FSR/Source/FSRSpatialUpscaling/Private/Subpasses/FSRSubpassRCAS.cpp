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
#include "FSRSubpassRCAS.h"

static int32 GFSR_RCAS = 1;
static FAutoConsoleVariableRef CVarFSRAddRCAS(
	TEXT("r.FidelityFX.FSR.RCAS.Enabled"),
	GFSR_RCAS,
	TEXT("FidelityFX FSR/RCAS : Robust Contrast Adaptive Sharpening Filter. Requires r.FidelityFX.FSR.PrimaryUpscale 1 or r.FidelityFX.FSR.SecondaryUpscale 1"),
	ECVF_RenderThreadSafe);

static float GFSR_Sharpness = 0.2; // 0.2 stops = 1 / (2^N) ~= 0.87 in lienar [0-1] sharpening of CAS
static FAutoConsoleVariableRef CVarFSRRCASSharpness(
	TEXT("r.FidelityFX.FSR.RCAS.Sharpness"),
	GFSR_Sharpness,
	TEXT("FidelityFX RCAS Sharpness in stops (0: sharpest, 1: 1/2 as sharp, 2: 1/4 as sharp, 3: 1/8 as sharp, etc.). A value of 0.2 would correspond to a ~0.87 sharpness in [0-1] linear scale"),
	ECVF_RenderThreadSafe);

static int32 GFSR_RCASDenoise = 0;
static FAutoConsoleVariableRef CVarFSRRCASDenoise(
	TEXT("r.FidelityFX.FSR.RCAS.Denoise"),
	GFSR_RCASDenoise,
	TEXT("FidelityFX RCAS Denoise support for grainy input such as dithered images or input with custom film grain effects applied prior to FSR. 1:On, 0:Off"),
	ECVF_RenderThreadSafe);

///
/// RCAS COMPUTE SHADER
///
class FRCASCS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FRCASCS);
	SHADER_USE_PARAMETER_STRUCT(FRCASCS, FGlobalShader);

	using FPermutationDomain = TShaderPermutationDomain<FFSR_UseFP16Dim, FFSR_OutputDeviceDim, FFSR_OutputDeviceNitsDim, FFSR_OutputDeviceConversion, FFSR_GrainDim, FRCAS_DenoiseDim>;

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT_INCLUDE(FRCASPassParameters, RCAS)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D, OutputTexture)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		FPermutationDomain PermutationVector(Parameters.PermutationId);
		const EFSR_OutputDevice eFSROutputDevice = PermutationVector.Get<FFSR_OutputDeviceDim>();

		if (PermutationVector.Get<FFSR_UseFP16Dim>())
		{
			if (Parameters.Platform != SP_PCD3D_SM5)
			{
				return false; //only compile FP16 shader permutation for Desktop PC.
			}
		}

		if (!PermutationVector.Get<FFSR_OutputDeviceConversion>() && eFSROutputDevice != EFSR_OutputDevice::sRGB)
		{
			return false; // don't compile FSR_OUTPUTDEVICE dimensions when there's no HDR conversions (== sRGB case)
		}

		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZEX"), 64);
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZEY"), 1);
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZEZ"), 1);
		OutEnvironment.SetDefine(TEXT("COMPUTE_SHADER"), 1);
	}
};

IMPLEMENT_GLOBAL_SHADER(FRCASCS, "/Plugin/FSR/Private/PostProcessFFX_RCAS.usf", "MainCS", SF_Compute);

///
/// RCAS PIXEL SHADER
///
class FRCASPS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FRCASPS);
	SHADER_USE_PARAMETER_STRUCT(FRCASPS, FGlobalShader);

	using FPermutationDomain = TShaderPermutationDomain<FFSR_UseFP16Dim, FFSR_OutputDeviceDim, FFSR_OutputDeviceNitsDim, FFSR_OutputDeviceConversion, FFSR_GrainDim, FRCAS_DenoiseDim>;

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT_INCLUDE(FRCASPassParameters, RCAS)
		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		FPermutationDomain PermutationVector(Parameters.PermutationId);
		const EFSR_OutputDevice eFSROutputDevice = PermutationVector.Get<FFSR_OutputDeviceDim>();

		if (PermutationVector.Get<FFSR_UseFP16Dim>())
		{
			if (Parameters.Platform != SP_PCD3D_SM5)
			{
				return false; //only compile FP16 shader permutation for Desktop PC.
			}
		}

		if (!PermutationVector.Get<FFSR_OutputDeviceConversion>() && eFSROutputDevice != EFSR_OutputDevice::sRGB)
		{
			return false; // don't compile FSR_OUTPUTDEVICE dimensions when there's no HDR conversions (== sRGB case)
		}

		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
	}
};

IMPLEMENT_GLOBAL_SHADER(FRCASPS, "/Plugin/FSR/Private/PostProcessFFX_RCAS.usf", "MainPS", SF_Pixel);

void FFSRSubpassRCAS::ParseEnvironment(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FInputs& PassInputs)
{
	Data->bRCASEnabled = GFSR_RCAS > 0;
	Data->bUSE_RCAS_DENOISE = GFSR_RCASDenoise > 0;
}

void FFSRSubpassRCAS::CreateResources(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FInputs& PassInputs)
{
	if (Data->bRCASEnabled)
	{
		const bool bRCASOutputsToLinearRT = Data->bHDRColorConversionPassEnabled && Data->bRCASIsTheLastPass
			&& (Data->eOUTPUT_DEVICE == EFSR_OutputDevice::scRGB || Data->eOUTPUT_DEVICE == EFSR_OutputDevice::Linear);

		FRDGTextureDesc RCASOutputTextureDesc = Data->UpscaleTexture.Texture->Desc;

		// if we have converted scRGB/Linear input to perceptual space before, we need to convert back to scRGB/Linear
		// again at the end of RCAS, hence, override the output format of RCAS from 10:10:10:2_UNORM to RGBA16F
		if (bRCASOutputsToLinearRT)
		{
			RCASOutputTextureDesc.Format = PassInputs.SceneColor.Texture->Desc.Format; /*PF_FloatRGBA*/
		}
		Data->SharpenedTexture = FScreenPassTexture(Data->UpscaleTexture);
		Data->SharpenedTexture.Texture = GraphBuilder.CreateTexture(RCASOutputTextureDesc, TEXT("FFX-RCAS-Output"), ERDGTextureFlags::MultiFrame);
	}
}

void FFSRSubpassRCAS::PostProcess(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FInputs& PassInputs)
{
	//
	// FidelityFX Robust Contrast Adaptive Sharpening (RCAS) Pass 
	//
	if (Data->bRCASEnabled)
	{
		AU1 const0[4]; // Configure FSR
		const bool bUSE_RCAS_GRAIN_INTENSITY = !Data->bChromaticAberrationPassEnabled && Data->bUSE_GRAIN_INTENSITY;

		FsrRcasCon(const0, GFSR_Sharpness);

		FScreenPassRenderTarget Output = Data->bChromaticAberrationPassEnabled
			? FScreenPassRenderTarget(Data->SharpenedTexture.Texture, ERenderTargetLoadAction::ENoAction)
			: PassInputs.OverrideOutput; // if no ChromAb, RCAS is last pass so override output
		if (!Output.IsValid())
		{
			Output = FScreenPassRenderTarget(Data->SharpenedTexture.Texture, ERenderTargetLoadAction::ENoAction);
		}
		const bool bOutputSupportsUAV = (Output.Texture->Desc.Flags & TexCreate_UAV) == TexCreate_UAV;

		// VS-PS
		if (!bOutputSupportsUAV || Data->bFORCE_VSPS)
		{
			FRCASPS::FParameters* PassParameters = GraphBuilder.AllocParameters<FRCASPS::FParameters>();

			// set pass inputs
			for (int i = 0; i < 4; i++) { PassParameters->RCAS.Const0[i] = const0[i]; }
			PassParameters->RCAS.InputTexture = Data->CurrentInputTexture;
			PassParameters->RCAS.FilmGrain = Data->FilmGrainParams;
			PassParameters->RCAS.VPColor_ExtentInverse = Data->PassOutputViewportParams.ExtentInverse;
			PassParameters->RenderTargets[0] = FRenderTargetBinding(Output.Texture, ERenderTargetLoadAction::ENoAction);

			// grab shaders
			FRCASPS::FPermutationDomain PSPermutationVector;
			PSPermutationVector.Set<FFSR_UseFP16Dim>(Data->bUSE_FP16);
			PSPermutationVector.Set<FFSR_GrainDim>(bUSE_RCAS_GRAIN_INTENSITY);
			PSPermutationVector.Set<FRCAS_DenoiseDim>(Data->bUSE_RCAS_DENOISE);
			
			PSPermutationVector.Set<FFSR_OutputDeviceConversion>(Data->bHDRColorConversionPassEnabled && Data->bRCASIsTheLastPass);
			if (PSPermutationVector.Get<FFSR_OutputDeviceConversion>())
			{
				PSPermutationVector.Set<FFSR_OutputDeviceDim>(Data->eOUTPUT_DEVICE);
				PSPermutationVector.Set<FFSR_OutputDeviceNitsDim>(Data->eMAX_NITS);
			}

			TShaderMapRef<FRCASPS> PixelShader(View.ShaderMap, PSPermutationVector);

			AddDrawScreenPass(GraphBuilder,
				RDG_EVENT_NAME("FidelityFX-FSR/RCAS Sharpness=%.2f OutputDevice=%d GrainIntensity=%d (PS)"
					, GFSR_Sharpness
					, Data->TonemapperOutputDeviceParameters.OutputDevice
					, ((bUSE_RCAS_GRAIN_INTENSITY) ? 1 : 0)),
				View, Data->OutputViewport, Data->OutputViewport,
				PixelShader, PassParameters,
				EScreenPassDrawFlags::None
			);
		}
		// CS
		else
		{
			FRCASCS::FParameters* PassParameters = GraphBuilder.AllocParameters<FRCASCS::FParameters>();

			for (int i = 0; i < 4; i++)
			{
				PassParameters->RCAS.Const0[i] = const0[i];
			}
			PassParameters->RCAS.InputTexture = Data->CurrentInputTexture;
			PassParameters->RCAS.FilmGrain = Data->FilmGrainParams;
			PassParameters->RCAS.VPColor_ExtentInverse = Data->PassOutputViewportParams.ExtentInverse;
			PassParameters->OutputTexture = GraphBuilder.CreateUAV(Data->SharpenedTexture.Texture);

			FRCASCS::FPermutationDomain CSPermutationVector;
			CSPermutationVector.Set<FFSR_UseFP16Dim>(Data->bUSE_FP16);
			CSPermutationVector.Set<FRCAS_DenoiseDim>(Data->bUSE_RCAS_DENOISE);
			CSPermutationVector.Set<FFSR_GrainDim>(bUSE_RCAS_GRAIN_INTENSITY);
			CSPermutationVector.Set<FFSR_OutputDeviceConversion>(Data->bHDRColorConversionPassEnabled && Data->bRCASIsTheLastPass);
			if (CSPermutationVector.Get<FFSR_OutputDeviceConversion>())
			{
				CSPermutationVector.Set<FFSR_OutputDeviceDim>(Data->eOUTPUT_DEVICE);
				CSPermutationVector.Set<FFSR_OutputDeviceNitsDim>(Data->eMAX_NITS);
			}

			TShaderMapRef<FRCASCS> ComputeShaderRCASPass(View.ShaderMap, CSPermutationVector);
			FComputeShaderUtils::AddPass(
				GraphBuilder,
				RDG_EVENT_NAME("FidelityFX-FSR/RCAS Sharpness=%.2f OutputDevice=%d GrainIntensity=%d (CS)"
					, GFSR_Sharpness
					, Data->TonemapperOutputDeviceParameters.OutputDevice 
					, ((bUSE_RCAS_GRAIN_INTENSITY) ? 1 : 0)),
				ComputeShaderRCASPass,
				PassParameters,
				FComputeShaderUtils::GetGroupCount(Data->OutputViewport.Rect.Size(), 16));
		}

		Data->FinalOutput = Output;
		Data->CurrentInputTexture = Output.Texture;
	}
}