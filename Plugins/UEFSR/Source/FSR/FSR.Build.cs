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
using UnrealBuildTool;

public class FSR : ModuleRules
{
	public FSR(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

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
				
				// ... add other public dependencies that you statically link with here ...
			}
			);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"Renderer",

				"FSRSpatialUpscaling",
				// ... add private dependencies that you statically link with here ...	
			}
			);

		DynamicallyLoadedModuleNames.AddRange(
		new string[]
		{
			// ... add any modules that your module loads dynamically here ...
		}
		);

		PrecompileForTargets = PrecompileTargetsType.Any;

		// Some things changed from 4.27 to 5.0 and from 5.0 to 5.1, so we r copying the Version.h to Shaders folder as a .ush file
		// To allow adjust the logic of the shaders based on the unreal engine version and this copy will occur when build the plugin
        string VersionHeaderPath = System.IO.Path.Combine(EngineDirectory, "Source", "Runtime", "Launch", "Resources", "Version.h");
        string DestinationVersionFile = System.IO.Path.Combine(ModuleDirectory, "..", "..", "Shaders", "Private", "Version.ush");
        
        // Version.h is read only by default, we need to uncheck this attribute to allow overwrite the existing Version.ush on copy
        if (System.IO.File.Exists(DestinationVersionFile))
        {
            System.IO.File.SetAttributes(DestinationVersionFile, System.IO.File.GetAttributes(DestinationVersionFile) & ~System.IO.FileAttributes.ReadOnly);
        }
        
        System.IO.File.Copy(VersionHeaderPath, DestinationVersionFile, true);
    }
}
