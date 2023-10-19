// Copyright Epic Games, Inc. All Rights Reserved.

#include "HeatMapPostInit.h"

#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include  "ShaderCore.h"

#define LOCTEXT_NAMESPACE "FHeatMapPostInit"

void FHeatMapPostInitModule::StartupModule()
{
	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("HeatMap"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/Plugin/HeatMap"), PluginShaderDir);
}

void FHeatMapPostInitModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FHeatMapPostInitModule, HeatMapPostInit)
