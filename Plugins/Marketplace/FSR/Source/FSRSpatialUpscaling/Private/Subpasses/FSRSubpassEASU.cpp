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
#include "FSRSubpassEASU.h"

static int32 GFSR_FP16 = 1;
static FAutoConsoleVariableRef CVarFSRUseFP16(
	TEXT("r.FidelityFX.FSR.UseFP16"),
	GFSR_FP16,
	TEXT("Enables half precision shaders for extra performance (if the device supports) without perceivable visual difference."),
	ECVF_RenderThreadSafe);

// debug cvars -------------------------------------------------------------
static int32 GFSR_ForcePS = 0;
static FAutoConsoleVariableRef CVarFSRForcePS(
	TEXT("r.FidelityFX.FSR.Debug.ForcePS"),
	GFSR_ForcePS,
	TEXT("Run FSR and RCAS in VS-PS"),
	ECVF_RenderThreadSafe);

//---------------------------------------------------------------------------------------------------
// ! IMPORTANT NOTE !
// Some NVidia GPUs show image corruption issues on DX11 when FP16 path is enabled with FSR.
// To ensure the correctness of the FSR pass output, we've added an additional boolean
// check here to determine whether or not to use FP16 for the DX11/NVidia path.
// Once this is no longer an issue, the default value of GFSR_FP16OnNvidiaDX11 can be set to 1
// or this block of code can be entirely removed.
static int32 GFSR_FP16OnNvidiaDX11 = 0;
static FAutoConsoleVariableRef CVarFSREnableFP16OnNvidiaDX11(
	TEXT("r.FidelityFX.FSR.EnableFP16OnNvDX11"),
	GFSR_FP16OnNvidiaDX11,
	TEXT("Enables FP16 path for DX11/NVidia GPUs, requires 'r.FidelityFX.FSR.UseFP16 1'. Default=0 as image corruption is seen on DX11/NVidia with FP16"),
	ECVF_RenderThreadSafe);
//---------------------------------------------------------------------------------------------------

///
/// FSR COMPUTE SHADER
///
class FFSRCS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FFSRCS);
	SHADER_USE_PARAMETER_STRUCT(FFSRCS, FGlobalShader);

	using FPermutationDomain = TShaderPermutationDomain<FFSR_UseFP16Dim, FFSR_OutputDeviceDim, FFSR_OutputDeviceNitsDim, FFSR_OutputDeviceConversion, FFSR_GrainDim>;

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT_INCLUDE(FFSRPassParameters, FSR)
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
		FPermutationDomain PermutationVector(Parameters.PermutationId);
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZEX"), 64);
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZEY"), 1);
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZEZ"), 1);
		OutEnvironment.SetDefine(TEXT("COMPUTE_SHADER"), 1);
	}
};

IMPLEMENT_GLOBAL_SHADER(FFSRCS, "/Plugin/FSR/Private/PostProcessFFX_FSR.usf", "MainCS", SF_Compute);

///
/// FSR PIXEL SHADER
///
class FFSRPS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FFSRPS);
	SHADER_USE_PARAMETER_STRUCT(FFSRPS, FGlobalShader);

	using FPermutationDomain = TShaderPermutationDomain<FFSR_UseFP16Dim, FFSR_OutputDeviceDim, FFSR_OutputDeviceNitsDim, FFSR_OutputDeviceConversion, FFSR_GrainDim>;

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT_INCLUDE(FFSRPassParameters, FSR)
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

IMPLEMENT_GLOBAL_SHADER(FFSRPS, "/Plugin/FSR/Private/PostProcessFFX_FSR.usf", "MainPS", SF_Pixel);

void FFSRSubpassEASU::ParseEnvironment(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FInputs& PassInputs)
{
	const bool bPlatformPC_D3D = View.GetShaderPlatform() == SP_PCD3D_SM5;
	Data->bUSE_FP16 = (GFSR_FP16 > 0) && bPlatformPC_D3D;
	Data->bFORCE_VSPS = GFSR_ForcePS > 0;

	//---------------------------------------------------------------------------------------------------
	// ! IMPORTANT NOTE !
	// Some NVidia GPUs show image corruption issues on DX11 when FP16 path is enabled with FSR.
	// To ensure the correctness of the FSR pass output, we've added an additional boolean
	// check here to determine whether or not to use FP16 for the DX11/NVidia path.
	// Once this is no longer an issue, the default value of GFSR_FP16OnNvidiaDX11 can be set to 1
	// or this block of code can be entirely removed.
#if PLATFORM_WINDOWS 
	if (IsRHIDeviceNVIDIA())
	{
		static const bool bIsDX11 = FCString::Strcmp(GDynamicRHI->GetName(), TEXT("D3D11")) == 0;
		if (bIsDX11)
		{
			Data->bUSE_FP16 = Data->bUSE_FP16 && (GFSR_FP16OnNvidiaDX11 > 0);
		}
	}
#endif
	//---------------------------------------------------------------------------------------------------
}

void FFSRSubpassEASU::CreateResources(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FInputs& PassInputs)
{
	Data->UpscaleTexture = FScreenPassTexture(PassInputs.SceneColor);
	
	Data->UpscaleTexture.Texture = GraphBuilder.CreateTexture(Data->FSROutputTextureDesc, TEXT("FFX-FSR-Output"), ERDGTextureFlags::MultiFrame);
	Data->UpscaleTexture.ViewRect = View.UnscaledViewRect;

	Data->OutputViewport = FScreenPassTextureViewport(Data->UpscaleTexture);
	Data->InputViewport = FScreenPassTextureViewport(PassInputs.SceneColor);
	Data->bEASUEnabled = (Data->InputViewport.Rect != Data->OutputViewport.Rect);

	Data->PassOutputViewportParams = GetScreenPassTextureViewportParameters(Data->OutputViewport);
}

void FFSRSubpassEASU::Upscale(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FInputs& PassInputs)
{
	//
	// FidelityFX Super Resolution (FSR) Pass
	//
	if (Data->bEASUEnabled)
	{
		AU1 const0[4];
		AU1 const1[4];
		AU1 const2[4];
		AU1 const3[4]; // Configure FSR

		FsrEasuConOffset(const0, const1, const2, const3,
			static_cast<AF1>(Data->InputViewport.Rect.Width())
			, static_cast<AF1>(Data->InputViewport.Rect.Height()) // current frame render resolution
			, static_cast<AF1>(PassInputs.SceneColor.Texture->Desc.Extent.X)
			, static_cast<AF1>(PassInputs.SceneColor.Texture->Desc.Extent.Y) // input container resolution (for DRS)
			, static_cast<AF1>(Data->OutputViewport.Rect.Width())
			, static_cast<AF1>(Data->OutputViewport.Rect.Height()) // upscaled-to resolution
			, static_cast<AF1>(Data->InputViewport.Rect.Min.X)
			, static_cast<AF1>(Data->InputViewport.Rect.Min.Y)				
		);

		const bool bUseIntermediateRT = (Data->bRCASEnabled || Data->bChromaticAberrationPassEnabled) || !PassInputs.OverrideOutput.IsValid();

		FScreenPassRenderTarget Output = bUseIntermediateRT
			? FScreenPassRenderTarget(Data->UpscaleTexture.Texture, Data->UpscaleTexture.ViewRect, ERenderTargetLoadAction::ENoAction)
			: PassInputs.OverrideOutput;

		const bool bOutputSupportsUAV = (Output.Texture->Desc.Flags & TexCreate_UAV) == TexCreate_UAV;

		// VS-PS
		if (!bOutputSupportsUAV || Data->bFORCE_VSPS)
		{
			FFSRPS::FParameters* PassParameters = GraphBuilder.AllocParameters<FFSRPS::FParameters>();
			for (int i = 0; i < 4; i++)
			{
				PassParameters->FSR.Const0[i] = const0[i];
				PassParameters->FSR.Const1[i] = const1[i];
				PassParameters->FSR.Const2[i] = const2[i];
				PassParameters->FSR.Const3[i] = const3[i];
			}
			PassParameters->FSR.InputTexture = Data->CurrentInputTexture;
			PassParameters->FSR.samLinearClamp = TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI();
			PassParameters->FSR.FilmGrain = Data->FilmGrainParams;
			PassParameters->FSR.VPColor_ExtentInverse = Data->PassOutputViewportParams.ExtentInverse;
			PassParameters->FSR.VPColor_ViewportMin = Data->PassOutputViewportParams.ViewportMin;
			PassParameters->RenderTargets[0] = FRenderTargetBinding(Output.Texture, ERenderTargetLoadAction::ENoAction);

			FFSRPS::FPermutationDomain PSPermutationVector;
			PSPermutationVector.Set<FFSR_UseFP16Dim>(Data->bUSE_FP16);
			PSPermutationVector.Set<FFSR_GrainDim>(Data->bUSE_GRAIN_INTENSITY && Data->bEASUIsTheLastPass);
			PSPermutationVector.Set<FFSR_OutputDeviceConversion>(Data->bHDRColorConversionPassEnabled && Data->bEASUIsTheLastPass);
			if (PSPermutationVector.Get<FFSR_OutputDeviceConversion>())
			{
				PSPermutationVector.Set<FFSR_OutputDeviceDim>(Data->eOUTPUT_DEVICE);
				PSPermutationVector.Set<FFSR_OutputDeviceNitsDim>(Data->eMAX_NITS);
			}

			const bool bFSRWithGrain = PSPermutationVector.Get<FFSR_GrainDim>();
			TShaderMapRef<FFSRPS> PixelShader(View.ShaderMap, PSPermutationVector);

			AddDrawScreenPass(GraphBuilder,
				RDG_EVENT_NAME("FidelityFX-FSR/Upscale %dx%d -> %dx%d GrainIntensity=%d (PS)"
					, Data->InputViewport.Rect.Width(), Data->InputViewport.Rect.Height()
					, Data->OutputViewport.Rect.Width(), Data->OutputViewport.Rect.Height()
					, (bFSRWithGrain ? 1 : 0)),
				View, Data->OutputViewport, Data->InputViewport,
				PixelShader, PassParameters,
				EScreenPassDrawFlags::None
			);
		}
		// CS
		else
		{
			FFSRCS::FParameters* PassParameters = GraphBuilder.AllocParameters<FFSRCS::FParameters>();

			for (int i = 0; i < 4; i++)
			{
				PassParameters->FSR.Const0[i] = const0[i];
				PassParameters->FSR.Const1[i] = const1[i];
				PassParameters->FSR.Const2[i] = const2[i];
				PassParameters->FSR.Const3[i] = const3[i];
			}
			PassParameters->FSR.InputTexture = Data->CurrentInputTexture;
			PassParameters->FSR.samLinearClamp = TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI();
			PassParameters->FSR.FilmGrain = Data->FilmGrainParams;
			PassParameters->FSR.VPColor_ExtentInverse = Data->PassOutputViewportParams.ExtentInverse;
			PassParameters->FSR.VPColor_ViewportMin = Data->PassOutputViewportParams.ViewportMin;
			PassParameters->OutputTexture = GraphBuilder.CreateUAV(Data->UpscaleTexture.Texture);

			FFSRCS::FPermutationDomain CSPermutationVector;
			CSPermutationVector.Set<FFSR_UseFP16Dim>(Data->bUSE_FP16);
			CSPermutationVector.Set<FFSR_GrainDim>(Data->bUSE_GRAIN_INTENSITY && Data->bEASUIsTheLastPass);
			CSPermutationVector.Set<FFSR_OutputDeviceConversion>(Data->bHDRColorConversionPassEnabled && Data->bEASUIsTheLastPass);
			if (CSPermutationVector.Get<FFSR_OutputDeviceConversion>())
			{
				CSPermutationVector.Set<FFSR_OutputDeviceDim>(Data->eOUTPUT_DEVICE);
				CSPermutationVector.Set<FFSR_OutputDeviceNitsDim>(Data->eMAX_NITS);
			}

			const bool bFSRWithGrain = CSPermutationVector.Get<FFSR_GrainDim>();

			TShaderMapRef<FFSRCS> ComputeShaderFSR(View.ShaderMap, CSPermutationVector);
			FComputeShaderUtils::AddPass(
				GraphBuilder,
				RDG_EVENT_NAME("FidelityFX-FSR/Upscale %dx%d -> %dx%d GrainIntensity=%d (CS)"
					, Data->InputViewport.Rect.Width(), Data->InputViewport.Rect.Height()
					, Data->OutputViewport.Rect.Width(), Data->OutputViewport.Rect.Height()
					, (bFSRWithGrain ? 1 : 0)),
				ComputeShaderFSR,
				PassParameters,
				FComputeShaderUtils::GetGroupCount(Data->OutputViewport.Rect.Size(), 16));

		}
		Data->FinalOutput = Output; // RCAS will override this if enabled
		Data->CurrentInputTexture = Output.Texture;
	}
}