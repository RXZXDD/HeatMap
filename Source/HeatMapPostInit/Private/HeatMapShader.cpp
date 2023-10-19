#include "HeatMapShader.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "Engine/TextureRenderTarget2D.h"
#include "RHIStaticStates.h"

IMPLEMENT_GLOBAL_SHADER(FHeatMapShader, "/Plugin/HeatMap/Private/HeatmapDrawer.usf", "MainCS", SF_Compute);

static void RenderHeatMap_RenderThread(FRHICommandListImmediate& RHICmdList, FTextureRenderTargetResource* RenderTargetResource,const TArray<FHeatMapInputParam>& InParams)
{
	check(IsInRenderingThread());

	//set up CS1 group dispatch 
	uint32 GGroupSize = 32;

	uint32 GroupSizeX = FMath::DivideAndRoundUp((uint32)RenderTargetResource->GetSizeX(), GGroupSize);
	uint32 GroupSizeY = FMath::DivideAndRoundUp((uint32)RenderTargetResource->GetSizeY(), GGroupSize);
	FIntVector GroupCount(GroupSizeX,GroupSizeY, 1);

	TShaderMapRef<FHeatMapShader> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

	TRefCountPtr<IPooledRenderTarget> PooledRenderTarget;
	
	{
		FRDGBuilder GraphBuilder(RHICmdList);

		//RDG_EVENT_SCOPE(GraphBuilder,"MyCSPass");
		//RDG_GPU_STAT_SCOPE(GraphBuilder, MyPassGPUStatScope);
		//RDG_CSV_STAT_EXCLUSIVE_SCOPE(GraphBuilder, "MyPass");

		auto PassParam =  GraphBuilder.AllocParameters<FHeatMapShader::FMyParameters>();
		FRDGTexture* RDGTex = GraphBuilder.CreateTexture(
										FRDGTextureDesc::Create2D(FIntPoint(RenderTargetResource->GetSizeX(),
											RenderTargetResource->GetSizeY()),
											RenderTargetResource->GetRenderTargetTexture()->GetFormat(),FClearValueBinding::White,
											ETextureCreateFlags::RenderTargetable|TexCreate_ShaderResource |TexCreate_UAV),
											TEXT("HeatMapTex"));

		FRDGTextureUAVDesc UAVDesc(RDGTex, 0);
		FRDGTextureUAV* UAV = GraphBuilder.CreateUAV(UAVDesc);
		PassParam->RWOutputSurface = UAV;
		PassParam->Total = RenderTargetResource->GetSizeX();
		PassParam->Sampler = TStaticSamplerState<ESamplerFilter::SF_Bilinear>::GetRHI();

		const int DataNum = InParams.Num();
		
		PassParam->DataNum = DataNum;

		
		for(int i = 0; i < DataNum; i++)
		{
			PassParam->input_Coord[i] = FVector4f(InParams[i].In_Coord.X, InParams[i].In_Coord.Y,0 ,0);
			PassParam->input_Val[i] = FVector4f(InParams[i].In_RGB.X, InParams[i].In_RGB.Y, InParams[i].In_RGB.Z, 0);
		}
		
		PassParam->TextureSize = FMath::Max(RenderTargetResource->GetSizeX(),RenderTargetResource->GetSizeY());
		
		GraphBuilder.AddPass(
			RDG_EVENT_NAME("WeatherHeatMapDraw"),
			PassParam,
			ERDGPassFlags::Compute,
			[ComputeShader, PassParam, GroupCount](FRHICommandList& RHICmdList)
			{
				RHICmdList.SetComputeShader(ComputeShader.GetComputeShader());
				SetShaderParameters(RHICmdList, ComputeShader, ComputeShader.GetComputeShader(), *PassParam);
				FComputeShaderUtils::Dispatch(RHICmdList, ComputeShader, *PassParam, GroupCount);
			}
		);

		//add extract queue
		GraphBuilder.QueueTextureExtraction(RDGTex, &PooledRenderTarget);


		//execute RDG
		GraphBuilder.Execute();

	}

	//Copy result to pass-in RT 
	//TODO Maybe a issue cause copy RT right after RDG exectue, and CS may not finish at this timing,may be have a delegate?
	RHICmdList.CopyTexture(PooledRenderTarget->GetRenderTargetItem().ShaderResourceTexture,
		RenderTargetResource->GetRenderTargetTexture() ,
		FRHICopyTextureInfo());

}

void FHeatMapRenderer::Render(UTextureRenderTarget2D* RenderTarget2D, TArray<FHeatMapInputParam>& InParams)
{
	if(RenderTarget2D )
	{
		FTextureRenderTargetResource* RenderTargetResource = RenderTarget2D->GameThread_GetRenderTargetResource();

		ENQUEUE_RENDER_COMMAND(RenderHeatMapCmd)
		(
			[RenderTargetResource, InParams](FRHICommandListImmediate& RHICmdList)
			{
				RenderHeatMap_RenderThread(RHICmdList, RenderTargetResource, InParams);
			}
		);
	}
}
