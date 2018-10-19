// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Patrick Kellmann

using UnrealBuildTool;
using System;
using System.IO;

public class LibTypeIIRML : ModuleRules
{
	private string ModulePath
	{
		get { return ModuleDirectory; }
	}
	
	private string ThirdPartyPath
	{
		get { return Path.Combine(ModulePath, "../ThirdParty"); }
	}
	
	public LibTypeIIRML(ReadOnlyTargetRules Target) : base(Target)
	{

		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		bEnableUndefinedIdentifierWarnings = false;
		
		string LibTypeIIRMLPath = Path.Combine(ThirdPartyPath, "TypeIIRML");

		PublicIncludePaths.AddRange(
		new string[] {
			// ... add public include paths required here ...
		}
		);
				
		
		PrivateIncludePaths.AddRange(
		new string[] {
			// ... add other private include paths required here ...
		}
		);
			
		
		PublicDependencyModuleNames.AddRange(
		new string[]
		{
			"Core",
			"Projects", 
			// ... add other public dependencies that you statically link with here ...
		}
		);
			
		
		PrivateDependencyModuleNames.AddRange(
		new string[]
		{

			// ... add private dependencies that you statically link with here ...	
		}
		);
		
		
		DynamicallyLoadedModuleNames.AddRange(
		new string[]
		{
			// ... add any modules that your module loads dynamically here ...
		}
		);

        // load the .lib file of the library.
        if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			PublicAdditionalLibraries.Add(Path.Combine(LibTypeIIRMLPath, "lib", "TypeIIRML.lib"));
			PublicIncludePaths.Add(Path.Combine(LibTypeIIRMLPath, "include"));

			PublicDefinitions.Add("WITH_TypeIIRML=1");
		}
	}
}
