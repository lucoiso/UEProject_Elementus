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
#include "FSRSubpassChromaticAberration.h"

static TAutoConsoleVariable<int32> CVarFSRPostFSRChromaticAberration(
	TEXT("r.FidelityFX.FSR.Post.ExperimentalChromaticAberration"),
#if WITH_EDITOR
	// Editor widgets also show chromatic aberration if chromatic aberration is applied after FSR.
	// So we keep it 0 for editor builds by default.
	0,
#else
	// Note: We've noticed some image issues such as jagged edges when FSR.Post.ExperimentalChromaticAberration is 1
	//       from the earlier integrations. Even though we addressed a number of issues with this release, 
	//       we still need additional time to make sure all of the outstanding issues are properly addressed. 
	//       Until the next release, we set the default for this option as 0 to keep the feature as an experimental one.
	0,
#endif
	TEXT("[Experimental] Applies ChromaticAberration after FidelityFX Super Resolution instead of during tonemapping for sharper images: It may produce slightly different fringes than the original.")
	TEXT("Note: widgets will show fringes if used in editor."),
	ECVF_RenderThreadSafe);

static FVector4 GetChromaticAberrationParameters(const FPostProcessSettings& PostProcessSettings)
{
	// this section copy-pasted from PostProcessTonemap.cpp to re-generate Color Fringe parameters ----------------
	FVector4 ChromaticAberrationParams;
	{
		// for scene color fringe
		// from percent to fraction
		float Offset = 0.0f;
		float StartOffset = 0.0f;
		float Multiplier = 1.0f;

		if (PostProcessSettings.ChromaticAberrationStartOffset < 1.0f - KINDA_SMALL_NUMBER)
		{
			Offset = PostProcessSettings.SceneFringeIntensity * 0.01f;
			StartOffset = PostProcessSettings.ChromaticAberrationStartOffset;
			Multiplier = 1.0f / (1.0f - StartOffset);
		}

		// Wavelength of primaries in nm
		const float PrimaryR = 611.3f;
		const float PrimaryG = 549.1f;
		const float PrimaryB = 464.3f;

		// Simple lens chromatic aberration is roughly linear in wavelength
		float ScaleR = 0.007f * (PrimaryR - PrimaryB);
		float ScaleG = 0.007f * (PrimaryG - PrimaryB);
		ChromaticAberrationParams = FVector4(Offset * ScaleR * Multiplier, Offset * ScaleG * Multiplier, StartOffset, 0.f);
	}
	// this section copy-pasted from PostProcessTonemap.cpp to re-generate Color Fringe parameters ----------------
	return ChromaticAberrationParams;
}

#if ENGINE_MAJOR_VERSION >= 5
static void GetLensParameters(FVector4f& LensPrincipalPointOffsetScale, FVector4f& LensPrincipalPointOffsetScaleInverse, const FViewInfo& View)
#else
static void GetLensParameters(FVector4& LensPrincipalPointOffsetScale, FVector4& LensPrincipalPointOffsetScaleInverse, const FViewInfo& View)
#endif
{
	// this section copy-pasted from PostProcessTonemap.cpp to re-generate Color Fringe parameters ----------------
	LensPrincipalPointOffsetScale = View.LensPrincipalPointOffsetScale;

	// forward transformation from shader:
	//return LensPrincipalPointOffsetScale.xy + UV * LensPrincipalPointOffsetScale.zw;

	// reverse transformation from shader:
	//return UV*(1.0f/LensPrincipalPointOffsetScale.zw) - LensPrincipalPointOffsetScale.xy/LensPrincipalPointOffsetScale.zw;

	LensPrincipalPointOffsetScaleInverse.X = -View.LensPrincipalPointOffsetScale.X / View.LensPrincipalPointOffsetScale.Z;
	LensPrincipalPointOffsetScaleInverse.Y = -View.LensPrincipalPointOffsetScale.Y / View.LensPrincipalPointOffsetScale.W;
	LensPrincipalPointOffsetScaleInverse.Z = 1.0f / View.LensPrincipalPointOffsetScale.Z;
	LensPrincipalPointOffsetScaleInverse.W = 1.0f / View.LensPrincipalPointOffsetScale.W;
	// this section copy-pasted from PostProcessTonemap.cpp to re-generate Color Fringe parameters ----------------
}

//
// STANDALONE CHROMATIC ABERRATION / PIXEL SHADER
//
class FChromaticAberrationPS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FChromaticAberrationPS);
	SHADER_USE_PARAMETER_STRUCT(FChromaticAberrationPS, FGlobalShader);

	using FPermutationDomain = TShaderPermutationDomain<FFSR_OutputDeviceDim, FFSR_OutputDeviceNitsDim, FFSR_GrainDim>;

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_TEXTURE(Texture2D, ColorTexture)
		SHADER_PARAMETER_SAMPLER(SamplerState, ColorSampler)

#if ENGINE_MAJOR_VERSION >= 5
		SHADER_PARAMETER(FVector4f, ChromaticAberrationParams)
		SHADER_PARAMETER(FVector4f, LensPrincipalPointOffsetScale)
		SHADER_PARAMETER(FVector4f, LensPrincipalPointOffsetScaleInverse)
#else	
		SHADER_PARAMETER(FVector4, ChromaticAberrationParams)
		SHADER_PARAMETER(FVector4, LensPrincipalPointOffsetScale)
		SHADER_PARAMETER(FVector4, LensPrincipalPointOffsetScaleInverse)
#endif
	
		SHADER_PARAMETER_STRUCT_INCLUDE(FFSRPassParameters_Grain, FilmGrain)
		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, Color)
		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		FPermutationDomain PermutationVector(Parameters.PermutationId);
		const EFSR_OutputDevice eFSROutputDevice = PermutationVector.Get<FFSR_OutputDeviceDim>();

		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
	}
};

IMPLEMENT_GLOBAL_SHADER(FChromaticAberrationPS, "/Plugin/FSR/Private/PostProcessChromaticAberration.usf", "MainPS", SF_Pixel);

//
// STANDALONE CHROMATIC ABERRATION / COMPUTE SHADER
//
class FChromaticAberrationCS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FChromaticAberrationCS);
	SHADER_USE_PARAMETER_STRUCT(FChromaticAberrationCS, FGlobalShader);

	using FPermutationDomain = TShaderPermutationDomain<FFSR_OutputDeviceDim, FFSR_OutputDeviceNitsDim, FFSR_GrainDim>;

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_TEXTURE(Texture2D, ColorTexture)
		SHADER_PARAMETER_SAMPLER(SamplerState, ColorSampler)
	
#if ENGINE_MAJOR_VERSION >= 5
		SHADER_PARAMETER(FVector4f, ChromaticAberrationParams)
		SHADER_PARAMETER(FVector4f, LensPrincipalPointOffsetScale)
		SHADER_PARAMETER(FVector4f, LensPrincipalPointOffsetScaleInverse)
#else	
		SHADER_PARAMETER(FVector4, ChromaticAberrationParams)
		SHADER_PARAMETER(FVector4, LensPrincipalPointOffsetScale)
		SHADER_PARAMETER(FVector4, LensPrincipalPointOffsetScaleInverse)
#endif
	
		SHADER_PARAMETER_STRUCT_INCLUDE(FFSRPassParameters_Grain, FilmGrain)
		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, Color)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D, OutputTexture)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		FPermutationDomain PermutationVector(Parameters.PermutationId);
		const EFSR_OutputDevice eFSROutputDevice = PermutationVector.Get<FFSR_OutputDeviceDim>();

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

IMPLEMENT_GLOBAL_SHADER(FChromaticAberrationCS, "/Plugin/FSR/Private/PostProcessChromaticAberration.usf", "MainCS", SF_Compute);

void FFSRSubpassChromaticAberration::ParseEnvironment(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FInputs& PassInputs)
{
	static TConsoleVariableData<int32>* CVarPostFSRColorFringe = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.FidelityFX.FSR.Post.ExperimentalChromaticAberration"));
	Data->bChromaticAberrationPassEnabled = CVarPostFSRColorFringe->GetValueOnRenderThread() > 0 && Data->PostProcess_SceneFringeIntensity > 0.01f;
}

void FFSRSubpassChromaticAberration::CreateResources(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FInputs& PassInputs)
{
	if (Data->bChromaticAberrationPassEnabled)
	{
		const bool bChromAbPassOutputsToLinearRT = Data->bHDRColorConversionPassEnabled
			&& (Data->eOUTPUT_DEVICE == EFSR_OutputDevice::scRGB || Data->eOUTPUT_DEVICE == EFSR_OutputDevice::Linear);

		FRDGTextureDesc ChromAbOutputTextureDesc = Data->UpscaleTexture.Texture->Desc;

		// if we have converted scRGB/Linear input to perceptual space before, we need to convert back to scRGB/Linear
		// again at the end of ChromAb, hence, override the output format of ChromAb from 10:10:10:2_UNORM to RGBA16F
		if (bChromAbPassOutputsToLinearRT)
		{
			ChromAbOutputTextureDesc.Format = PassInputs.SceneColor.Texture->Desc.Format; /*PF_FloatRGBA*/
		}
		Data->ColorFringeOutputTexture = FScreenPassTexture(Data->UpscaleTexture);
		Data->ColorFringeOutputTexture.Texture = GraphBuilder.CreateTexture(ChromAbOutputTextureDesc, TEXT("FFX-ChromAb-Output"), ERDGTextureFlags::MultiFrame);

		// setup a valid PostProcessSettings object so we can invoke GetChromaticAberrationParameters and GetLensParameters against it
		Data->ChromaticAberrationPostProcessSettings = View.FinalPostProcessSettings;
		Data->ChromaticAberrationPostProcessSettings.SceneFringeIntensity = Data->PostProcess_SceneFringeIntensity;
		Data->ChromaticAberrationPostProcessSettings.ChromaticAberrationStartOffset = Data->PostProcess_ChromaticAberrationStartOffset;
	}
}

void FFSRSubpassChromaticAberration::PostProcess(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FInputs& PassInputs)
{
	//
	// Post-upscale/sharpen Chromatic Aberration Pass
	//
	if (Data->bChromaticAberrationPassEnabled)
	{
		FRHISamplerState* BilinearClampSampler = TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI();

		FScreenPassRenderTarget Output = PassInputs.OverrideOutput;
		if (!Output.IsValid())
		{
			Output = FScreenPassRenderTarget(Data->ColorFringeOutputTexture.Texture, ERenderTargetLoadAction::ENoAction);
		}

		const bool bOutputSupportsUAV = (Output.Texture->Desc.Flags & TexCreate_UAV) == TexCreate_UAV;

		// VS-PS
		if (!bOutputSupportsUAV)
		{
			FChromaticAberrationPS::FParameters* PassParameters = GraphBuilder.AllocParameters<FChromaticAberrationPS::FParameters>();

			// set pass inputs
			PassParameters->ColorTexture = Data->CurrentInputTexture;
			PassParameters->ColorSampler = BilinearClampSampler;
			PassParameters->RenderTargets[0] = FRenderTargetBinding(Output.Texture, ERenderTargetLoadAction::ENoAction);
			PassParameters->Color = GetScreenPassTextureViewportParameters(FScreenPassTextureViewport(PassParameters->ColorTexture));

#if ENGINE_MAJOR_VERSION >= 5
			PassParameters->ChromaticAberrationParams = FVector4f(GetChromaticAberrationParameters(Data->ChromaticAberrationPostProcessSettings));
#else
			PassParameters->ChromaticAberrationParams = GetChromaticAberrationParameters(Data->ChromaticAberrationPostProcessSettings);			
#endif
			
			PassParameters->FilmGrain = Data->FilmGrainParams;
			GetLensParameters(PassParameters->LensPrincipalPointOffsetScale, PassParameters->LensPrincipalPointOffsetScaleInverse, View);

			// grab shaders
			FChromaticAberrationPS::FPermutationDomain PSPermutationVector;
			PSPermutationVector.Set<FFSR_GrainDim>(Data->bUSE_GRAIN_INTENSITY);
			PSPermutationVector.Set<FFSR_OutputDeviceDim>(Data->eOUTPUT_DEVICE);
			if (Data->bHDRColorConversionPassEnabled)
			{
				PSPermutationVector.Set<FFSR_OutputDeviceNitsDim>(Data->eMAX_NITS);
			}

			TShaderMapRef<FChromaticAberrationPS> PixelShader(View.ShaderMap, PSPermutationVector);

			AddDrawScreenPass(GraphBuilder,
				RDG_EVENT_NAME("Post-FidelityFX ChromaticAberration GrainIntensity=%d OutputDevice=%d (PS)"
					, ((Data->bUSE_GRAIN_INTENSITY) ? 1 : 0)
					, Data->TonemapperOutputDeviceParameters.OutputDevice
				), View, Data->OutputViewport, Data->OutputViewport,
				PixelShader, PassParameters,
				EScreenPassDrawFlags::None
			);
		}
		// CS
		else
		{
			FChromaticAberrationCS::FParameters* PassParameters = GraphBuilder.AllocParameters<FChromaticAberrationCS::FParameters>();

			// set pass inputs
			PassParameters->ColorTexture = Data->CurrentInputTexture;
			PassParameters->ColorSampler = BilinearClampSampler;
			PassParameters->Color = GetScreenPassTextureViewportParameters(FScreenPassTextureViewport(PassParameters->ColorTexture));
			
#if ENGINE_MAJOR_VERSION >= 5
			PassParameters->ChromaticAberrationParams = FVector4f(GetChromaticAberrationParameters(Data->ChromaticAberrationPostProcessSettings));
#else
			PassParameters->ChromaticAberrationParams = GetChromaticAberrationParameters(Data->ChromaticAberrationPostProcessSettings);			
#endif
			
			PassParameters->OutputTexture = GraphBuilder.CreateUAV(Output.Texture);
			PassParameters->FilmGrain = Data->FilmGrainParams;
			
			GetLensParameters(PassParameters->LensPrincipalPointOffsetScale, PassParameters->LensPrincipalPointOffsetScaleInverse, View);

			FChromaticAberrationCS::FPermutationDomain CSPermutationVector;
			CSPermutationVector.Set<FFSR_GrainDim>(Data->bUSE_GRAIN_INTENSITY);
			CSPermutationVector.Set<FFSR_OutputDeviceDim>(Data->eOUTPUT_DEVICE);
			if (Data->bHDRColorConversionPassEnabled)
			{
				CSPermutationVector.Set<FFSR_OutputDeviceNitsDim>(Data->eMAX_NITS);
			}

			TShaderMapRef<FChromaticAberrationCS> ComputeShaderRCASPass(View.ShaderMap, CSPermutationVector);

			FComputeShaderUtils::AddPass(
				GraphBuilder,
				RDG_EVENT_NAME("Post-FidelityFX ChromaticAberration GrainIntensity=%d OutputDevice=%d (CS)"
					, ((Data->bUSE_GRAIN_INTENSITY) ? 1 : 0)
					, Data->TonemapperOutputDeviceParameters.OutputDevice
				),
				ComputeShaderRCASPass,
				PassParameters,
				FComputeShaderUtils::GetGroupCount(Data->OutputViewport.Rect.Size(), 16));
		}

		Data->FinalOutput = Output;
	}
}