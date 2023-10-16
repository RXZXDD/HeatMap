// Copyright Epic Games, Inc. All Rights Reserved.

#include "HeatMapBPLibrary.h"
#include "HeatMapShader.h"

UHeatMapBPLibrary::UHeatMapBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}


void UHeatMapBPLibrary::DoRenderHeatMap(UTextureRenderTarget2D* RenderTarget2D,
	TArray<FHeatMapInputParam> InParams)
{
	if(RenderTarget2D && InParams.Num()>0)
	{
		FHeatMapRenderer HeatMapRenderer;
		HeatMapRenderer.Render(RenderTarget2D, InParams);
	}
}
