using UnrealBuildTool;

public class DialogueMemoryTable : ModuleRules
{
    public DialogueMemoryTable(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        RuntimeDependencies.Add("$(PluginDir)/Resources/DialogueMemorySummarizerPrompt.md", StagedFileType.UFS);

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "HTTP",
                "Json",
                "JsonUtilities"
            }
        );
    }
}
