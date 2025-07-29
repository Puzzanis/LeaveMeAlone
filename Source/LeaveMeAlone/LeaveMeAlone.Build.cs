// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class LeaveMeAlone : ModuleRules
{
	public LeaveMeAlone(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"LeaveMeAlone",
			"LeaveMeAlone/Variant_Platforming",
			"LeaveMeAlone/Variant_Combat",
			"LeaveMeAlone/Variant_Combat/AI",
			"LeaveMeAlone/Variant_SideScrolling",
			"LeaveMeAlone/Variant_SideScrolling/Gameplay",
			"LeaveMeAlone/Variant_SideScrolling/AI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
