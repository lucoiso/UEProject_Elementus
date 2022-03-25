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
#include "FSRSubpassHDR.h"

#include "BlueNoise.h"

static float GFSR_HDR_PQDither = 1.0f;
static FAutoConsoleVariableRef CVarFSR_HDR_PQDither(
	TEXT("r.FidelityFX.FSR.HDR.PQDitherAmount"),
	GFSR_HDR_PQDither,
	TEXT("[HDR-Only] DitherAmount to apply for PQ->Gamma2 conversion to eliminate color banding, when the output device is ST2084/PQ."),
	ECVF_RenderThreadSafe);

static EFSR_OutputDevice GetFSROutputDevice(ETonemapperOutputDevice UE4TonemapperOutputDevice)
{
	EFSR_OutputDevice ColorSpace = EFSR_OutputDevice::MAX;

	switch (UE4TonemapperOutputDevice)
	{
	case ETonemapperOutputDevice::sRGB:
	case ETonemapperOutputDevice::Rec709:
	case ETonemapperOutputDevice::ExplicitGammaMapping:
		ColorSpace = EFSR_OutputDevice::sRGB;
		break;
	case ETonemapperOutputDevice::ACES1000nitST2084:
	case ETonemapperOutputDevice::ACES2000nitST2084:
		ColorSpace = EFSR_OutputDevice::PQ;
		break;
	case ETonemapperOutputDevice::ACES1000nitScRGB:
	case ETonemapperOutputDevice::ACES2000nitScRGB:
		ColorSpace = EFSR_OutputDevice::scRGB;
		break;
	case ETonemapperOutputDevice::LinearEXR:
	case ETonemapperOutputDevice::LinearNoToneCurve:
	case ETonemapperOutputDevice::LinearWithToneCurve:
		ColorSpace = EFSR_OutputDevice::Linear;
		break;
	}

	return ColorSpace;
}

static EFSR_OutputDeviceMaxNits GetOutputDeviceMaxNits(ETonemapperOutputDevice eDevice) // only needed for HDR color conversions
{
	// this will select shader variants so MAX cannot be used.
	EFSR_OutputDeviceMaxNits eMaxNits = EFSR_OutputDeviceMaxNits::EFSR_1000Nits;  // Assume 1000 for the default case.

	switch (eDevice)
	{
	case ETonemapperOutputDevice::ACES1000nitST2084:
	case ETonemapperOutputDevice::ACES1000nitScRGB:
		eMaxNits = EFSR_OutputDeviceMaxNits::EFSR_1000Nits;
		break;
	case ETonemapperOutputDevice::ACES2000nitST2084:
	case ETonemapperOutputDevice::ACES2000nitScRGB:
		eMaxNits = EFSR_OutputDeviceMaxNits::EFSR_2000Nits;
		break;

	case ETonemapperOutputDevice::sRGB:
	case ETonemapperOutputDevice::Rec709:
	case ETonemapperOutputDevice::ExplicitGammaMapping:
	case ETonemapperOutputDevice::LinearEXR:
	case ETonemapperOutputDevice::LinearNoToneCurve:
	case ETonemapperOutputDevice::LinearWithToneCurve:
	default:
		// noop
		break;
	}
	return eMaxNits;
}

//
// COLOR CONVERSION COMPUTE SHADER
//
class FColorConversionCS : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FColorConversionCS);
	SHADER_USE_PARAMETER_STRUCT(FColorConversionCS, FGlobalShader);

	using FPermutationDomain = TShaderPermutationDomain<FFSR_OutputDeviceNitsDim, FFSR_OutputDeviceDim, FFSR_PQDitherDim>;

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_TEXTURE(Texture2D, InputTexture)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D, OutputTexture)
		SHADER_PARAMETER_STRUCT_REF(FBlueNoise, BlueNoise)
		SHADER_PARAMETER(float, DitherAmount)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		FPermutationDomain PermutationVector(Parameters.PermutationId);

		if (PermutationVector.Get<FFSR_OutputDeviceDim>() == EFSR_OutputDevice::sRGB)
		{
			return false; // no color conversion is needed for sRGB
		}

		if (PermutationVector.Get<FFSR_OutputDeviceDim>() != EFSR_OutputDevice::PQ)
		{
			if (PermutationVector.Get<FFSR_PQDitherDim>())
			{
				return false; // do not compile PQ variants for non-PQ output devices
			}
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
IMPLEMENT_GLOBAL_SHADER(FColorConversionCS, "/Plugin/FSR/Private/PostProcessFFX_HDRColorConversion.usf", "MainCS", SF_Compute);

void FFSRSubpassHDR::ParseEnvironment(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FInputs& PassInputs)
{
	Data->TonemapperOutputDeviceParameters = GetTonemapperOutputDeviceParameters(*View.Family);

	const ETonemapperOutputDevice eTonemapperOutputDevice = (ETonemapperOutputDevice)Data->TonemapperOutputDeviceParameters.OutputDevice;
	Data->eOUTPUT_DEVICE = GetFSROutputDevice(eTonemapperOutputDevice);
	Data->eMAX_NITS = GetOutputDeviceMaxNits(eTonemapperOutputDevice);

	Data->bHDRColorConversionPassEnabled = Data->eOUTPUT_DEVICE != EFSR_OutputDevice::sRGB;
}

void FFSRSubpassHDR::CreateResources(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FInputs& PassInputs)
{
	Data->ColorConvertedTexture = FScreenPassTexture(PassInputs.SceneColor);

	if (Data->bHDRColorConversionPassEnabled)
	{
		// FSR expects to work in a perceptual space, preferrably Gamma2.
		// For the HDR outputs, we will need to convert to Gamma2 prior to FSR.
		// 
		// scRGB & Linear
		// --------------
		// Since we will convert scRGB into a perceptual space and store in 10:10:10:2_UNORM format 
		// a Gamma2 encoded color w/ Rec2020 primaries, override the FSR output texture desc format here.
		// Although, override it only when FSR upscaling is not the last pass, becase RCAS or ChromAb may still
		// utilize the sRGB / 10:10:10:2 input for faster loads, and convert back to scRGB at the end of their invocation.
		// Otherwise, FSR pass will load 10:10:10:2 input, and convert back to scRGB before storing out.
		//
		// PQ
		// --------------
		// The RT format will already be 10:10:10:2, we just need to change the encoding during color conversion.
		if (!Data->bEASUIsTheLastPass)
		{
			Data->FSROutputTextureDesc.Format = PF_A2B10G10R10;
		}

		// prepare color conversion resources
		FRDGTextureDesc ColorConversionTextureDesc = PassInputs.SceneColor.Texture->Desc;
		ColorConversionTextureDesc.Format = PF_A2B10G10R10;
		ColorConversionTextureDesc.Flags = TexCreate_ShaderResource | TexCreate_UAV | TexCreate_RenderTargetable;
		Data->ColorConvertedTexture.Texture = GraphBuilder.CreateTexture(ColorConversionTextureDesc, TEXT("FFX-ColorConversion-Output"), ERDGTextureFlags::MultiFrame);
		Data->ColorConvertedTexture.ViewRect = PassInputs.SceneColor.ViewRect;

		Data->bHDRColorConversionPassRan = false;
	}
}

void FFSRSubpassHDR::Upscale(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FInputs& PassInputs)
{
	//
	// [HDR-only] Color Conversion Pass
	//
	if (Data->bHDRColorConversionPassEnabled)
	{
		// --------------------------------------------------------------------------------------
		// HDR/scRGB
		// ---------------------------------------------------------------------------------------
		// In scRGB, there could be input ranging between [-0.5, 12.5].
		// scRGB uses sRGB primaries and is a linear color space, however,
		// FSR expects perceptual space (preferrably Gamma2/sRGB) and values in [0-1] range.
		// 
		// Here, we will convert the scRGB input into a Gamma2 encoded perceptual space. 
		// During the conversion, we will change the sRGB primaries into Rec2020 primaries 
		// and clip the remaining negative values, while also normalizing [0,1]
		// 
		// ---------------------------------------------------------------------------------------
		// HDR/PQ
		// ---------------------------------------------------------------------------------------
		// The render target format won't change, although the signal encoding will be Gamma2.
		// UE4 tonemapper will output normalized (depending on 1000 or 2000 nits) PQ space color.
		// The conversion shader will do the following and store the value: PQ -> Linear -> Gamma2
		// 
		// ---------------------------------------------------------------------------------------
		// HDR/Linear
		// ---------------------------------------------------------------------------------------
		// Linear can contain values in [0, FP16_MAX)
		// we will use FSR/SRTM (Simple Reversible Tonemapper) to normalize the lienar values
		// before storing in a 10:10:10:2_UNORM intermediate target at the end of this pass.
		//
		// ======================================================================================
		// 
		// The last pass of the FSR chain will convert back to the original device output space.
		//
		// ======================================================================================

		// Set parameters
		FColorConversionCS::FParameters* PassParameters = GraphBuilder.AllocParameters<FColorConversionCS::FParameters>();
		PassParameters->InputTexture = Data->CurrentInputTexture;
		PassParameters->OutputTexture = GraphBuilder.CreateUAV(Data->ColorConvertedTexture.Texture);
		FBlueNoise BlueNoise;
		InitializeBlueNoise(BlueNoise);
		PassParameters->BlueNoise = CreateUniformBufferImmediate(BlueNoise, EUniformBufferUsage::UniformBuffer_SingleFrame);
		PassParameters->DitherAmount = FMath::Min(1.0f, FMath::Max(0.0f, GFSR_HDR_PQDither));

		// Set shader variant
		FColorConversionCS::FPermutationDomain CSPermutationVector;
		CSPermutationVector.Set<FFSR_OutputDeviceDim>(Data->eOUTPUT_DEVICE);
		CSPermutationVector.Set<FFSR_OutputDeviceNitsDim>(Data->eMAX_NITS);
		if (Data->eOUTPUT_DEVICE == EFSR_OutputDevice::PQ)
		{
			CSPermutationVector.Set<FFSR_PQDitherDim>(PassParameters->DitherAmount > 0);
		}

		// Dispatch
		TShaderMapRef<FColorConversionCS> ComputeShaderFSR(View.ShaderMap, CSPermutationVector);
		FComputeShaderUtils::AddPass(
			GraphBuilder,
			RDG_EVENT_NAME("FidelityFX-FSR/HDRColorConversion (Gamma2) OutputDevice=%d DeviceMaxNits=%d (CS)"
				, Data->TonemapperOutputDeviceParameters.OutputDevice
				, (Data->eMAX_NITS == EFSR_OutputDeviceMaxNits::EFSR_1000Nits ? 1000 : 2000)
			),
			ComputeShaderFSR,
			PassParameters,
			FComputeShaderUtils::GetGroupCount(Data->InputViewport.Rect.Size(), 16)
		);

		Data->CurrentInputTexture = Data->ColorConvertedTexture.Texture;

		Data->bHDRColorConversionPassRan = true;
	}
}

void FFSRSubpassHDR::PostProcess(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FInputs& PassInputs)
{
	// it is possible for the Upscale pass to have been skipped (like if DRS has the screen percentage at 100).  if that happened but it was needed, run it now.
	if (Data->bHDRColorConversionPassEnabled && !Data->bHDRColorConversionPassRan)
	{
		Upscale(GraphBuilder, View, PassInputs);
	}
}