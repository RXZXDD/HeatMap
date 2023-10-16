// Copyright Epic Games, Inc. All Rights Reserved.

#include "HeatMap.h"

#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FHeatMapModule"

void FHeatMapModule::StartupModule()
{
	
	
}

void FHeatMapModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FHeatMapModule, HeatMap)