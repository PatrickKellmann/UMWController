// C// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Patrick Kellmann

#include "UROSBaseControllerMW.h"

#define LOCTEXT_NAMESPACE "FUROSBaseControllerMWModule"

void FUROSBaseControllerMWModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FUROSBaseControllerMWModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUROSBaseControllerMWModule, UROSBaseControllerMW)