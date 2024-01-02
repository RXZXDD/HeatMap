#pragma once
#include "CoreMinimal.h"
#include "GlobalShader.h"
#include "ShaderParameterStruct.h"
#include "DataDrivenShaderPlatformInfo.h"
#include "HeatMapShader.generated.h"

#define MAX_DATA_NUM 2000


USTRUCT(BlueprintType)
struct FHeatMapInputParam
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite,EditAnywhere, Category="HeatMapParam")
	FVector4 In_Coord;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="HeatMapParam")
	FVector3f In_RGB;
};

class FHeatMapShader : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FHeatMapShader);
	SHADER_USE_PARAMETER_STRUCT(FHeatMapShader, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FMyParameters, )
		SHADER_PARAMETER_SAMPLER(SamplerState, Sampler)//8
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D, RWOutputSurface)//8
		SHADER_PARAMETER(uint32, Total) //1
		SHADER_PARAMETER(uint32, DataNum) //1
		SHADER_PARAMETER(uint32, TextureSize) //1
		SHADER_PARAMETER_ARRAY(FVector4f, input_Coord, [MAX_DATA_NUM])
		SHADER_PARAMETER_ARRAY(FVector4f, input_Val, [MAX_DATA_NUM])
	END_SHADER_PARAMETER_STRUCT()

		using FParameters = FMyParameters;

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZE_X"), 32);
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZE_Y"), 32);
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZE_Z"), 1);
		OutEnvironment.SetDefine(TEXT("MAX_DATA_NUM"), MAX_DATA_NUM);

	}
};

class HEATMAPPOSTINIT_API FHeatMapRenderer
{
public:
	FHeatMapRenderer() {};
	
	void Render(class UTextureRenderTarget2D* RenderTarget2D, TArray<FHeatMapInputParam>& InParams);

};

