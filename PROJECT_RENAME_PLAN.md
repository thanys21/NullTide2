# Project/module rename plan: NullTide2 鈫?NullTide

Status: **Planning only. No rename implemented.** The only file written for this request is this report.

## Scope and scan method

Scanned the current project root `C:/Users/loith/Documents/Unreal Projects/NullTide2`, including hidden and ignored files. Initial file enumeration returned 1,184 files. Used case-insensitive ripgrep searches for NullTide2/NULLTIDE2_API (which also cover /Script/NullTide2, NullTide2Editor, NullTide2Target and quoted module names), a separate filename/path scan, and a UTF-16LE byte-pattern pass. Binary assets were read for matching bytes only, never edited. The appendix lists every detected matching file/path and assigns its occurrences a category; build-critical text lines are listed separately.

All 496 Git object files were also searched as raw bytes; none yielded a literal match. Compressed Git history was not decompressed or semantically searched. Git history is not part of the rename surface. Binary counts are byte-pattern detections, not decoded Unreal reference counts; compressed/encoded references can escape this scan. The six asset matches agree with the earlier MCP inventory audit, but this pass does not claim a fresh complete Unreal asset-reference traversal.

The source tree has one runtime module and two targets. No additional project module or Plugins directory was found. AGENTS.md requires reference checking before removing/renaming Blueprint-exposed APIs; keep the C++ class names unchanged and redirect the module package.

User reports baseline stabilization complete. The existing baseline report still says build pending; treat that report as historical, not current build evidence. No build was run for this planning request.

## Classification

1. **Must rename** 鈥?authored project/module identifiers and their source paths.
2. **Must preserve temporarily for CoreRedirect** 鈥?serialized old native package/class references and future redirect OldName strings.
3. **Generated file that should be regenerated** 鈥?UBT/UHT outputs, IDE projects/caches, receipts and disposable editor settings.
4. **Documentation/history that does not affect the build** 鈥?prior reports, logs, crash history and Git internals.
5. **Suspicious and requires review** 鈥?remote URL, potentially valuable autosaves/source-control state, and legacy configuration anomalies.

Detected file/path totals: category 1: 10; category 2: 6; category 3: 176; category 4: 17; category 5: 6. Categories apply to all matching occurrences within each appendix row. A path-only row is included even when its content has no detected match. The project root itself is an additional path-level occurrence; all absolute descendants inherit it, so relative paths are used to avoid counting the same ancestor thousands of times.

## 1. Must rename

| Current path / location | Intended change |
|---|---|
| NullTide2.uproject | Rename to NullTide.uproject; Modules[0].Name becomes NullTide (line 8) |
| Source/NullTide2/ | Rename module directory to Source/NullTide/; move its existing contents unchanged except changes below |
| Source/NullTide2/NullTide2.Build.cs | Rename to Source/NullTide/NullTide.Build.cs; ModuleRules class and constructor become NullTide (lines 5, 7) |
| Source/NullTide2.Target.cs | Rename to Source/NullTide.Target.cs; class/constructor NullTide2Target 鈫?NullTideTarget (lines 6, 8); ExtraModuleNames entry 鈫?NullTide (line 13) |
| Source/NullTide2Editor.Target.cs | Rename to Source/NullTideEditor.Target.cs; class/constructor 鈫?NullTideEditorTarget (lines 6, 8); ExtraModuleNames entry 鈫?NullTide (line 13) |
| Source/NullTide2/NullTide2.h | Rename to Source/NullTide/NullTide.h; no old-name text match in header itself |
| Source/NullTide2/NullTide2.cpp | Rename to Source/NullTide/NullTide.cpp; include NullTide.h (line 3); both module token and game-name string in IMPLEMENT_PRIMARY_GAME_MODULE become NullTide (line 6) |
| Source/NullTide2/Content/InteractionSystem/Items/ItemDefinition.h:15 | NULLTIDE2_API 鈫?NULLTIDE_API |
| Source/NullTide2/Content/InteractionSystem/Fragments/InventoryItemFragment.h:13 | NULLTIDE2_API 鈫?NULLTIDE_API |

ItemDefinition and InventoryItemFragment filenames, generated-header include basenames, UItemDefinition/UInventoryItemFragment names, property names and flags remain unchanged. Their four source files move only because the module ancestor directory moves.

Do not leave a second old runtime module or define a compatibility NULLTIDE2_API macro merely to hide missed edits. CoreRedirects handle serialized identity; they do not repair build rules, linker exports or include paths.

### Physical project folder

Renaming the enclosing folder to `C:/Users/loith/Documents/Unreal Projects/NullTide` is optional for Unreal's logical project/module rename. It is a separate operational step requiring IDE/editor shutdown and workspace reopening. If a complete on-disk rename is desired, perform it last after validation or in a new checkout; verify the destination is absent and preserve source control. Never blanket-replace the ancestor path while files still live in NullTide2. Generated launch/build paths must reflect whichever root is actually chosen.

The current automation workspace/permissions point at the old directory. Moving outside that root can require a new workspace or permission approval during implementation. No folder move is authorized by this plan alone.

## 2. Preserve old serialized names through redirects

Changing the module changes the native package from /Script/NullTide2 to /Script/NullTide even when class names stay the same. Parent classes and class-typed properties in existing assets must resolve before any resave.

Six current binary assets contain the old name (INV = Content/LevelPrototyping/InventorySystem):

| Asset | Compatibility concern |
|---|---|
| INV/Items/Item_Wood.uasset | Native ItemDefinition parent / class metadata |
| INV/Items/Item_Sword.uasset | Native ItemDefinition parent / class metadata |
| INV/InventoryManagerComponent.uasset | InventoryItems array of ItemDefinition class references |
| INV/BP_PickUpItem.uasset | ItemDefinition class property / function pins |
| INV/UI/Widgets/W_Inventory.uasset | Native item class pin types in inventory enumeration / slot creation |
| INV/UI/Widgets/W_InventorySlot.uasset | ItemDefinition class property and GetClassDefaults icon access |

Do not rename these assets or change their bytes. Their old names remain valid through redirects. Characters, level external actors and other assets can depend transitively on them even without a literal old-module string; absence from the table does not eliminate validation requirements.

Proposed addition to Config/DefaultEngine.ini during the eventual rename:

```ini
[CoreRedirects]
+PackageRedirects=(OldName="/Script/NullTide2",NewName="/Script/NullTide")
```

Use an exact package redirect, without MatchSubstring/MatchWildcard, for this one-to-one module move. This also covers native types added to the old module before migration. Explicit class redirects, if testing reveals a need for class-specific handling, would be:

```ini
+ClassRedirects=(OldName="/Script/NullTide2.ItemDefinition",NewName="/Script/NullTide.ItemDefinition")
+ClassRedirects=(OldName="/Script/NullTide2.InventoryItemFragment",NewName="/Script/NullTide.InventoryItemFragment")
```

These are candidate alternatives/supplements for validation, not instructions to add redundant rules indiscriminately. Reflected names omit the C++ U prefix. Do not redirect the removed plain InventoryItemFragment or undeclared AInventoryItemFragment: they are not the surviving reflected types.

Local UE 5.8 evidence: Engine/Config/BaseEngine.ini documents CoreRedirects and full script class paths around lines 372鈥?82; Engine/Source/Runtime/CoreUObject/Private/UObject/CoreRedirects.cpp handles package redirects during non-package name lookup around lines 1933鈥?944 and 1981鈥?009.

Preserve old strings in redirect OldName fields. 鈥淭emporary鈥?does not imply removing them immediately after the first successful build: retain until all dependent assets, saved data and supported old content have been migrated and validated. A global zero-match test is the wrong acceptance criterion.

## 3. Regenerate generated and editor files

- Root NullTide2.sln/.slnx, Automation_NullTide2.sln/.slnx and NullTide2.code-workspace: regenerate for the renamed project. Automation solution filenames match even though their contents had no literal match.
- .vscode/c_cpp_properties.json, compileCommands_Default.json, compileCommands_NullTide2.json and response-file directories: regenerate. They contain absolute project paths, target labels, source filenames, generated includes and module definitions.
- .vs/NullTide2/: solution databases, file indexes, PCH cache, BlueprintCache, layout and .suo files are local IDE state. Preserve any desired personal settings before regeneration; never text-rewrite binary databases.
- Intermediate/: all matching UHT generated headers/cpp, module rules assemblies/manifests, target metadata, makefiles, response files, object/PCH/lib/exp/resource files, dependency and action caches, project files, cached asset registries and Live Coding data.
- Binaries/Win64/: UnrealEditor-NullTide2.dll/.pdb, NullTide2Editor.target and UnrealEditor.modules. Rebuild and regenerate receipts/module manifests; do not just rename the DLL.
- Saved/Config/WindowsEditor/EditorPerProjectUserSettings.ini: old import/open directories at lines 1788鈥?798 and module compile metadata at 3353鈥?354. Regenerate/reconcile personal settings after reopening.
- Saved/ShaderDebugInfo/.../DDCKey-Editor.txt: diagnostic output, regenerate if needed.
- DerivedDataCache had no literal match in this snapshot; it remains generated data, not a source-rename target.

No generated file edits are proposed. Do not indiscriminately delete Saved: autosaves, crash evidence and source-control state need preservation/review.

## 4. Documentation/history

Leave historical references in INVENTORY_AUDIT.md and INVENTORY_BASELINE_FIX.md intact or annotate their historical status later. Old commands and source paths describe the prior state; they are not live build inputs. The new report deliberately contains the old name as migration evidence.

Saved/Logs and Saved/Crashes names/content remain historical. They need neither renaming nor regeneration to make the module build. Crash dumps include UTF-16 strings as well as logs containing ordinary text.

.git/index and .git/FETCH_HEAD are Git-owned metadata: do not edit them. Ordinary future source-control rename operations update the index. Do not rewrite repository history to erase old paths.

## 5. Suspicious references and review decisions

| Location | Finding / required decision |
|---|---|
| .git/config:9 | origin points to https://github.com/thanys21/NullTide2.git. A module rename does not require a repository rename. Preserve this URL unless the remote repository is separately renamed and verified; do not assume NullTide.git exists. |
| Saved/Autosaves/Game/LevelPrototyping/InventorySystem/ | Four old-name autosave packages are listed in the appendix. Preserve until the user confirms no recoverable work remains. If later restored, they need the same redirect coverage as source assets. |
| Saved/SourceControl/UncontrolledChangelists.json | 90 literal occurrences in scan, primarily absolute paths. Preserve/reconcile editor-managed pending work; do not blanket-replace or discard. |
| Config/DefaultEngine.ini:2 | GameName=MyProject, not NullTide2. Review setting GameName=NullTide for consistent project identity; this is not an old-name match. |
| Config/DefaultEngine.ini:120鈥?21 | Existing ActiveGameNameRedirects map TP_TopDownBP and /Script/TP_TopDownBP to /Script/MyProject. Determine whether any legacy content relies on these. Do not change historical OldGameName identifiers to NullTide; if needed, point legacy destinations directly at the correct final module after validation. |
| Config/DefaultGame.ini:3 | ProjectName=Top Down BP Game Template. Review updating the display name to Null Tide or NullTide; preserve ProjectID unless there is a separate reason to change product identity. |
| Config/DefaultEditor.ini:2 | SimpleMapName=/Game/MyProject/Maps/TopDownExampleMap is a stale-looking template path. Validate separately; no automatic map/path rename. Current startup/default maps already use /Game/TopDown/Lvl_TopDown. |
| Generated IDE files | Earlier reads referenced the deleted Private/InventoryItemFragment.cpp; later reads changed counts, consistent with regeneration during inspection. Regenerate once tools are closed rather than treating stale generated references as source defects. |
| INVENTORY_BASELINE_FIX.md | Still records Live Coding build blocker despite user-reported completed stabilization. Preserve as history; obtain/record the successful baseline build evidence before migration rather than assuming this report is current. |

No /Script/NullTide2 config section, quoted NullTide2 config module value, or existing NullTide2 CoreRedirect was found in authored Config files. .codex/config.toml uses a localhost MCP endpoint and contains no old project name; AGENTS.md, .gitignore and .ignore also have no matching old-name text. No edits to these are indicated by the search.

## Implementation sequence 鈥?future work only

1. Record a known-good baseline commit/build and preserve unsaved editor work. Close editor/Live Coding and IDE processes that hold generated databases or DLLs.
2. Apply the authored descriptor, target, module, source filename and export macro changes together. Preserve native class/property and Blueprint asset names.
3. Add the exact module CoreRedirect before first loading old assets with the renamed native module.
4. Regenerate IDE projects and rebuild NullTideEditor Win64 Development from the renamed .uproject. Isolate/remove only verified disposable old build artifacts after preserving anything needed; do not broad-delete Saved or Content.
5. Open the renamed project; inspect load logs for missing /Script/NullTide2 imports and invalid parent classes. Through Unreal MCP, validate Item_Wood/Item_Sword parent resolution, Fragments reflection, manager array types, pickup definition defaults and widget graph pins. Include BP_TopDownCharacter, BP_TopDownController and Lvl_TopDown/external pickup instances in validation.
6. With authorization for the implementation phase, compile affected Blueprints and test pickup/UI behavior before deciding whether to resave assets. Compile/save through Unreal, never binary search-and-replace. No automatic asset migration is part of this plan.
7. Validate game target and packaging/cooking as appropriate. Check old script imports on a fresh editor launch and with regenerated caches; do not rely on a previously loaded DLL.
8. Optionally move the enclosing folder, reopen the workspace and regenerate absolute IDE paths. Remote repository rename is a separate decision.
9. Rerun the classified search. Remaining old names should be explained by redirect OldName values, untouched compatible serialized assets, historical evidence, or explicitly preserved recovery data.

## Acceptance criteria

- NullTide.uproject declares module NullTide; both targets and ModuleRules resolve consistently.
- Authored native code has no NULLTIDE2_API, old include/module registration or old target identifiers.
- UItemDefinition and UInventoryItemFragment retain their C++ names and property contracts; only their native package changes.
- Existing item Blueprints resolve through redirects without reparenting or renaming.
- Editor and game builds succeed, inventory behavior is unchanged, and load/cook checks do not report missing native classes.
- No Inventory V2 architecture work is mixed into the rename.
- Historical strings and recovery data are not mistaken for failed migration.

## Appendix A 鈥?build-critical text occurrences

```text
NullTide2.uproject:8:			"Name": "NullTide2",
Source\NullTide2Editor.Target.cs:6:public class NullTide2EditorTarget : TargetRules
Source\NullTide2Editor.Target.cs:8:	public NullTide2EditorTarget(TargetInfo Target) : base(Target)
Source\NullTide2Editor.Target.cs:13:		ExtraModuleNames.AddRange( new string[] { "NullTide2" } );
Source\NullTide2.Target.cs:6:public class NullTide2Target : TargetRules
Source\NullTide2.Target.cs:8:	public NullTide2Target(TargetInfo Target) : base(Target)
Source\NullTide2.Target.cs:13:		ExtraModuleNames.AddRange( new string[] { "NullTide2" } );
Source\NullTide2\NullTide2.Build.cs:5:public class NullTide2 : ModuleRules
Source\NullTide2\NullTide2.Build.cs:7:	public NullTide2(ReadOnlyTargetRules Target) : base(Target)
Source\NullTide2\NullTide2.cpp:3:#include "NullTide2.h"
Source\NullTide2\NullTide2.cpp:6:IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, NullTide2, "NullTide2" );
Source\NullTide2\Content\InteractionSystem\Items\ItemDefinition.h:15:class NULLTIDE2_API UItemDefinition : public UObject
Source\NullTide2\Content\InteractionSystem\Fragments\InventoryItemFragment.h:13:class NULLTIDE2_API UInventoryItemFragment : public UObject



```

## Appendix B 鈥?complete detected file/path classification

Counts are case-insensitive literal matches from the byte-oriented pass; zero means path-only or UTF-16-only detection. Multiple requested search terms overlap, so these are not additive per-query counts. Binary counts are not semantic reference counts. Tools were not frozen; generated files may change during a live session. All occurrences in a row use its category, with the exceptions and handling described above.

| Category | Relative file path | Literal matches | Old name in relative path |
|---|---|---:|---|
| 1 | `NullTide2.uproject` | 1 | Yes |
| 1 | `Source/NullTide2.Target.cs` | 3 | Yes |
| 1 | `Source/NullTide2/Content/InteractionSystem/Fragments/InventoryItemFragment.cpp` | 0 | Yes |
| 1 | `Source/NullTide2/Content/InteractionSystem/Fragments/InventoryItemFragment.h` | 1 | Yes |
| 1 | `Source/NullTide2/Content/InteractionSystem/Items/ItemDefinition.cpp` | 0 | Yes |
| 1 | `Source/NullTide2/Content/InteractionSystem/Items/ItemDefinition.h` | 1 | Yes |
| 1 | `Source/NullTide2/NullTide2.Build.cs` | 2 | Yes |
| 1 | `Source/NullTide2/NullTide2.cpp` | 3 | Yes |
| 1 | `Source/NullTide2/NullTide2.h` | 0 | Yes |
| 1 | `Source/NullTide2Editor.Target.cs` | 3 | Yes |
| 2 | `Content/LevelPrototyping/InventorySystem/BP_PickUpItem.uasset` | 1 | No |
| 2 | `Content/LevelPrototyping/InventorySystem/InventoryManagerComponent.uasset` | 1 | No |
| 2 | `Content/LevelPrototyping/InventorySystem/Items/Item_Sword.uasset` | 5 | No |
| 2 | `Content/LevelPrototyping/InventorySystem/Items/Item_Wood.uasset` | 5 | No |
| 2 | `Content/LevelPrototyping/InventorySystem/UI/Widgets/W_Inventory.uasset` | 1 | No |
| 2 | `Content/LevelPrototyping/InventorySystem/UI/Widgets/W_InventorySlot.uasset` | 1 | No |
| 3 | `.vs/NullTide2/BlueprintCache/NullTide2.json` | 161 | Yes |
| 3 | `.vs/NullTide2/FileContentIndex/18ccfc83-08a6-43a4-9c96-22b49cbe95c3.vsidx` | 1 | Yes |
| 3 | `.vs/NullTide2/FileContentIndex/2d40d10a-0064-4e9a-9c04-698bd9ea8299.vsidx` | 4 | Yes |
| 3 | `.vs/NullTide2/FileContentIndex/3066d0ed-f557-4d2b-9170-531d29d6a4d1.vsidx` | 0 | Yes |
| 3 | `.vs/NullTide2/FileContentIndex/31a56f64-d69c-4cb9-b3e9-e0f980248364.vsidx` | 0 | Yes |
| 3 | `.vs/NullTide2/FileContentIndex/3413ba56-bc55-484f-8316-ae67b079c617.vsidx` | 0 | Yes |
| 3 | `.vs/NullTide2/FileContentIndex/40549970-ec2d-4c8d-a452-c72e09a65561.vsidx` | 0 | Yes |
| 3 | `.vs/NullTide2/FileContentIndex/513ec403-eecd-4bf3-8551-e4d1dd93289e.vsidx` | 0 | Yes |
| 3 | `.vs/NullTide2/FileContentIndex/52cf565e-810a-45e7-bbe5-b095e3a03a9f.vsidx` | 0 | Yes |
| 3 | `.vs/NullTide2/FileContentIndex/54790198-c083-454b-8f2b-2968148f22b3.vsidx` | 0 | Yes |
| 3 | `.vs/NullTide2/FileContentIndex/56f9dabb-9eb2-4a67-b5ed-58cd895979d7.vsidx` | 0 | Yes |
| 3 | `.vs/NullTide2/FileContentIndex/60fce67e-10f3-483e-9bb9-57e48d1d130f.vsidx` | 0 | Yes |
| 3 | `.vs/NullTide2/FileContentIndex/6d0afb38-e00d-436c-be7b-8ae669adaed2.vsidx` | 0 | Yes |
| 3 | `.vs/NullTide2/FileContentIndex/744e004b-fbd7-4649-9caa-c5e8116d9c15.vsidx` | 0 | Yes |
| 3 | `.vs/NullTide2/FileContentIndex/7ef0aa13-aaae-43d7-866c-8a83f482ea52.vsidx` | 0 | Yes |
| 3 | `.vs/NullTide2/FileContentIndex/7f2ebdf4-da3a-4d22-b1de-168024779761.vsidx` | 0 | Yes |
| 3 | `.vs/NullTide2/FileContentIndex/8bde3fa5-7025-4aac-b2cf-ca3d1388f2aa.vsidx` | 0 | Yes |
| 3 | `.vs/NullTide2/FileContentIndex/8c10122d-2823-4ce2-a735-7d1119bb2155.vsidx` | 17 | Yes |
| 3 | `.vs/NullTide2/FileContentIndex/9210201a-6ae2-4e78-b4e9-8c4551426e39.vsidx` | 0 | Yes |
| 3 | `.vs/NullTide2/FileContentIndex/949d6b4f-175a-4037-a1ca-97980c8ad199.vsidx` | 0 | Yes |
| 3 | `.vs/NullTide2/FileContentIndex/a2cbb902-b9a6-4dd7-b42b-4c3bcd34f5e4.vsidx` | 0 | Yes |
| 3 | `.vs/NullTide2/FileContentIndex/a7d06773-87ce-4ca0-8c24-c6864b50c17e.vsidx` | 5 | Yes |
| 3 | `.vs/NullTide2/FileContentIndex/af0f0271-d0cd-4c5b-bb30-b33190aba352.vsidx` | 0 | Yes |
| 3 | `.vs/NullTide2/FileContentIndex/b2f61947-42f3-4ea6-ba92-43b9b66d85f7.vsidx` | 0 | Yes |
| 3 | `.vs/NullTide2/FileContentIndex/b8c1c675-4865-4391-94c8-7210017dfe5b.vsidx` | 0 | Yes |
| 3 | `.vs/NullTide2/FileContentIndex/d162179c-81ff-4f6f-ad15-b590e93fa8d5.vsidx` | 0 | Yes |
| 3 | `.vs/NullTide2/FileContentIndex/dc159fe1-3a44-4212-b38a-c97001a3eb7e.vsidx` | 0 | Yes |
| 3 | `.vs/NullTide2/FileContentIndex/e10646f8-ceb1-43f8-ade6-607cecf89e4b.vsidx` | 0 | Yes |
| 3 | `.vs/NullTide2/FileContentIndex/eed7912c-38ef-4ecf-9423-27ea17743f99.vsidx` | 0 | Yes |
| 3 | `.vs/NullTide2/FileContentIndex/f621fffc-2d7c-4e15-bc4b-fc0c6182056e.vsidx` | 16 | Yes |
| 3 | `.vs/NullTide2/FileContentIndex/f6ee8b75-748b-4b17-a3fd-fd79ab032890.vsidx` | 0 | Yes |
| 3 | `.vs/NullTide2/FileContentIndex/f87439f3-ada8-427c-9142-56114076b2d6.vsidx` | 0 | Yes |
| 3 | `.vs/NullTide2/v17/.suo` | 122 | Yes |
| 3 | `.vs/NullTide2/v17/Browse.VC.db` | 225 | Yes |
| 3 | `.vs/NullTide2/v17/DocumentLayout.backup.json` | 50 | Yes |
| 3 | `.vs/NullTide2/v17/DocumentLayout.json` | 28 | Yes |
| 3 | `.vs/NullTide2/v17/fileList.bin` | 43 | Yes |
| 3 | `.vs/NullTide2/v17/ipch/128a2bd78e918ae2.ipch` | 38 | Yes |
| 3 | `.vs/NullTide2/v17/ipch/48172ecaca379e0d.ipch` | 2 | Yes |
| 3 | `.vs/NullTide2/v17/Solution.VC.db` | 303 | Yes |
| 3 | `.vscode/c_cpp_properties.json` | 5 | No |
| 3 | `.vscode/compileCommands_Default.json` | 17 | No |
| 3 | `.vscode/compileCommands_Default/NullTide2.0.rsp` | 14 | Yes |
| 3 | `.vscode/compileCommands_NullTide2.json` | 21 | Yes |
| 3 | `.vscode/compileCommands_NullTide2/NullTide2.0.rsp` | 14 | Yes |
| 3 | `Automation_NullTide2.sln` | 0 | Yes |
| 3 | `Automation_NullTide2.slnx` | 0 | Yes |
| 3 | `Binaries/Win64/NullTide2Editor.target` | 5 | Yes |
| 3 | `Binaries/Win64/UnrealEditor-NullTide2.dll` | 3 | Yes |
| 3 | `Binaries/Win64/UnrealEditor-NullTide2.pdb` | 247 | Yes |
| 3 | `Binaries/Win64/UnrealEditor.modules` | 2 | No |
| 3 | `Intermediate/Build/BuildRules/NullTide2ModuleRules.dll` | 6 | Yes |
| 3 | `Intermediate/Build/BuildRules/NullTide2ModuleRules.pdb` | 4 | Yes |
| 3 | `Intermediate/Build/BuildRules/NullTide2ModuleRulesManifest.json` | 7 | Yes |
| 3 | `Intermediate/Build/BuildRulesProjects/NullTide2ModuleRules/NullTide2ModuleRules.csproj` | 8 | Yes |
| 3 | `Intermediate/Build/SourceFileCache.bin` | 13 | No |
| 3 | `Intermediate/Build/Win64/ActionHistory.bin` | 7 | No |
| 3 | `Intermediate/Build/Win64/NullTide2Editor/NullTide2Editor.deps` | 0 | Yes |
| 3 | `Intermediate/Build/Win64/NullTide2Editor/NullTide2Editor.uhtmanifest` | 17 | Yes |
| 3 | `Intermediate/Build/Win64/NullTide2Editor/NullTide2Editor.uhtpath` | 0 | Yes |
| 3 | `Intermediate/Build/Win64/NullTide2Editor/NullTide2Editor.uhtsettings` | 0 | Yes |
| 3 | `Intermediate/Build/Win64/NullTide2Editor/NullTide2Editor.uhttimestamps` | 1 | Yes |
| 3 | `Intermediate/Build/Win64/UnrealEditor/Inc/NullTide2/UHT/InventoryItemFragment.gen.cpp` | 12 | Yes |
| 3 | `Intermediate/Build/Win64/UnrealEditor/Inc/NullTide2/UHT/InventoryItemFragment.generated.h` | 19 | Yes |
| 3 | `Intermediate/Build/Win64/UnrealEditor/Inc/NullTide2/UHT/ItemDefinition.gen.cpp` | 13 | Yes |
| 3 | `Intermediate/Build/Win64/UnrealEditor/Inc/NullTide2/UHT/ItemDefinition.generated.h` | 19 | Yes |
| 3 | `Intermediate/Build/Win64/UnrealEditor/Inc/NullTide2/UHT/NullTide2.init.gen.cpp` | 11 | Yes |
| 3 | `Intermediate/Build/Win64/UnrealEditor/Inc/NullTide2/UHT/NullTide2Classes.h` | 0 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/ActionHistory.bin` | 8 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/Core/SharedDefinitions.Core.Cpp20.h` | 0 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/Core/SharedPCH.Core.Cpp20.cpp` | 0 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/Core/SharedPCH.Core.Cpp20.h` | 0 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/Core/SharedPCH.Core.Cpp20.h.obj.rsp` | 10 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/Core/SharedPCH.Core.Cpp20.h.obj.rsp.old` | 10 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/CoreUObject/SharedDefinitions.CoreUObject.Cpp20.h` | 0 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/CoreUObject/SharedDefinitions.CoreUObject.RTTI.Cpp20.h` | 0 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/CoreUObject/SharedPCH.CoreUObject.Cpp20.cpp` | 0 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/CoreUObject/SharedPCH.CoreUObject.Cpp20.h` | 0 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/CoreUObject/SharedPCH.CoreUObject.Cpp20.h.obj.rsp` | 10 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/CoreUObject/SharedPCH.CoreUObject.Cpp20.h.obj.rsp.old` | 10 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/CoreUObject/SharedPCH.CoreUObject.RTTI.Cpp20.cpp` | 0 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/CoreUObject/SharedPCH.CoreUObject.RTTI.Cpp20.h` | 0 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/CoreUObject/SharedPCH.CoreUObject.RTTI.Cpp20.h.obj.rsp` | 10 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/CoreUObject/SharedPCH.CoreUObject.RTTI.Cpp20.h.obj.rsp.old` | 10 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/Engine/SharedDefinitions.Engine.Cpp20.h` | 0 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/Engine/SharedDefinitions.Engine.Project.ValApi.ValExpApi.Cpp20.InclOrderUnreal5_6.h` | 0 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/Engine/SharedPCH.Engine.Cpp20.cpp` | 0 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/Engine/SharedPCH.Engine.Cpp20.h` | 0 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/Engine/SharedPCH.Engine.Cpp20.h.obj.rsp` | 10 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/Engine/SharedPCH.Engine.Cpp20.h.obj.rsp.old` | 10 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/Engine/SharedPCH.Engine.Project.ValApi.ValExpApi.Cpp20.InclOrderUnreal5_6.cpp` | 0 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/Engine/SharedPCH.Engine.Project.ValApi.ValExpApi.Cpp20.InclOrderUnreal5_6.h` | 0 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/Engine/SharedPCH.Engine.Project.ValApi.ValExpApi.Cpp20.InclOrderUnreal5_6.h.obj.rsp` | 10 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/Engine/SharedPCH.Engine.Project.ValApi.ValExpApi.Cpp20.InclOrderUnreal5_6.h.obj.rsp.old` | 10 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/Makefile.bin` | 974 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/Slate/SharedDefinitions.Slate.Cpp20.h` | 0 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/Slate/SharedPCH.Slate.Cpp20.cpp` | 0 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/Slate/SharedPCH.Slate.Cpp20.h` | 0 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/Slate/SharedPCH.Slate.Cpp20.h.obj.rsp` | 10 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/Slate/SharedPCH.Slate.Cpp20.h.obj.rsp.old` | 10 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/TargetMetadata.json` | 18 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/UnrealEd/SharedDefinitions.UnrealEd.Cpp20.h` | 0 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/UnrealEd/SharedDefinitions.UnrealEd.Project.ValApi.ValExpApi.Cpp20.InclOrderUnreal5_6.h` | 0 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/UnrealEd/SharedDefinitions.UnrealEd.RTTI.Cpp20.h` | 0 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/UnrealEd/SharedPCH.UnrealEd.Cpp20.cpp` | 0 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/UnrealEd/SharedPCH.UnrealEd.Cpp20.h` | 0 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/UnrealEd/SharedPCH.UnrealEd.Cpp20.h.obj.rsp` | 10 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/UnrealEd/SharedPCH.UnrealEd.Cpp20.h.obj.rsp.old` | 10 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/UnrealEd/SharedPCH.UnrealEd.Project.ValApi.ValExpApi.Cpp20.InclOrderUnreal5_6.cpp` | 0 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/UnrealEd/SharedPCH.UnrealEd.Project.ValApi.ValExpApi.Cpp20.InclOrderUnreal5_6.cpp.old` | 0 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/UnrealEd/SharedPCH.UnrealEd.Project.ValApi.ValExpApi.Cpp20.InclOrderUnreal5_6.h` | 0 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/UnrealEd/SharedPCH.UnrealEd.Project.ValApi.ValExpApi.Cpp20.InclOrderUnreal5_6.h.dep.json` | 6 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/UnrealEd/SharedPCH.UnrealEd.Project.ValApi.ValExpApi.Cpp20.InclOrderUnreal5_6.h.obj` | 18 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/UnrealEd/SharedPCH.UnrealEd.Project.ValApi.ValExpApi.Cpp20.InclOrderUnreal5_6.h.obj.rsp` | 10 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/UnrealEd/SharedPCH.UnrealEd.Project.ValApi.ValExpApi.Cpp20.InclOrderUnreal5_6.h.obj.rsp.old` | 10 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/UnrealEd/SharedPCH.UnrealEd.Project.ValApi.ValExpApi.Cpp20.InclOrderUnreal5_6.h.pch` | 18 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/UnrealEd/SharedPCH.UnrealEd.Project.ValApi.ValExpApi.Cpp20.InclOrderUnreal5_6.h.sarif` | 0 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/UnrealEd/SharedPCH.UnrealEd.RTTI.Cpp20.cpp` | 0 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/UnrealEd/SharedPCH.UnrealEd.RTTI.Cpp20.h` | 0 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/UnrealEd/SharedPCH.UnrealEd.RTTI.Cpp20.h.obj.rsp` | 10 | Yes |
| 3 | `Intermediate/Build/Win64/x64/NullTide2Editor/Development/UnrealEd/SharedPCH.UnrealEd.RTTI.Cpp20.h.obj.rsp.old` | 10 | Yes |
| 3 | `Intermediate/Build/Win64/x64/UnrealEditor/ActionHistory.bin` | 43 | No |
| 3 | `Intermediate/Build/Win64/x64/UnrealEditor/Development/FileHashCache.bin` | 6 | No |
| 3 | `Intermediate/Build/Win64/x64/UnrealEditor/Development/NullTide2/Default.rc2.res` | 0 + UTF-16 detected | Yes |
| 3 | `Intermediate/Build/Win64/x64/UnrealEditor/Development/NullTide2/Default.rc2.res.rsp` | 4 | Yes |
| 3 | `Intermediate/Build/Win64/x64/UnrealEditor/Development/NullTide2/Default.rc2.res.rsp.old` | 4 | Yes |
| 3 | `Intermediate/Build/Win64/x64/UnrealEditor/Development/NullTide2/Definitions.NullTide2.h` | 8 | Yes |
| 3 | `Intermediate/Build/Win64/x64/UnrealEditor/Development/NullTide2/InventoryItemFragment.cpp.obj.rsp` | 20 | Yes |
| 3 | `Intermediate/Build/Win64/x64/UnrealEditor/Development/NullTide2/InventoryItemFragment.cpp.obj.rsp.old` | 20 | Yes |
| 3 | `Intermediate/Build/Win64/x64/UnrealEditor/Development/NullTide2/ItemDefinition.cpp.dep.json` | 11 | Yes |
| 3 | `Intermediate/Build/Win64/x64/UnrealEditor/Development/NullTide2/ItemDefinition.cpp.obj` | 41 | Yes |
| 3 | `Intermediate/Build/Win64/x64/UnrealEditor/Development/NullTide2/ItemDefinition.cpp.obj.rsp` | 20 | Yes |
| 3 | `Intermediate/Build/Win64/x64/UnrealEditor/Development/NullTide2/ItemDefinition.cpp.sarif` | 0 | Yes |
| 3 | `Intermediate/Build/Win64/x64/UnrealEditor/Development/NullTide2/LiveCodingInfo.json` | 2 | Yes |
| 3 | `Intermediate/Build/Win64/x64/UnrealEditor/Development/NullTide2/LiveCodingInfo.json.old` | 2 | Yes |
| 3 | `Intermediate/Build/Win64/x64/UnrealEditor/Development/NullTide2/Module.NullTide2.gen.cpp` | 7 | Yes |
| 3 | `Intermediate/Build/Win64/x64/UnrealEditor/Development/NullTide2/Module.NullTide2.gen.cpp.dep.json` | 17 | Yes |
| 3 | `Intermediate/Build/Win64/x64/UnrealEditor/Development/NullTide2/Module.NullTide2.gen.cpp.obj` | 127 | Yes |
| 3 | `Intermediate/Build/Win64/x64/UnrealEditor/Development/NullTide2/Module.NullTide2.gen.cpp.obj.rsp` | 24 | Yes |
| 3 | `Intermediate/Build/Win64/x64/UnrealEditor/Development/NullTide2/Module.NullTide2.gen.cpp.old` | 5 | Yes |
| 3 | `Intermediate/Build/Win64/x64/UnrealEditor/Development/NullTide2/Module.NullTide2.gen.cpp.sarif` | 0 | Yes |
| 3 | `Intermediate/Build/Win64/x64/UnrealEditor/Development/NullTide2/NullTide2.cpp.dep.json` | 11 | Yes |
| 3 | `Intermediate/Build/Win64/x64/UnrealEditor/Development/NullTide2/NullTide2.cpp.obj` | 71 | Yes |
| 3 | `Intermediate/Build/Win64/x64/UnrealEditor/Development/NullTide2/NullTide2.cpp.obj.rsp` | 24 | Yes |
| 3 | `Intermediate/Build/Win64/x64/UnrealEditor/Development/NullTide2/NullTide2.cpp.sarif` | 0 | Yes |
| 3 | `Intermediate/Build/Win64/x64/UnrealEditor/Development/NullTide2/NullTide2.Shared.rsp` | 5 | Yes |
| 3 | `Intermediate/Build/Win64/x64/UnrealEditor/Development/NullTide2/NullTide2.Shared.rsp.old` | 9 | Yes |
| 3 | `Intermediate/Build/Win64/x64/UnrealEditor/Development/NullTide2/PerModuleInline.gen.cpp` | 0 | Yes |
| 3 | `Intermediate/Build/Win64/x64/UnrealEditor/Development/NullTide2/PerModuleInline.gen.cpp.dep.json` | 7 | Yes |
| 3 | `Intermediate/Build/Win64/x64/UnrealEditor/Development/NullTide2/PerModuleInline.gen.cpp.obj` | 35 | Yes |
| 3 | `Intermediate/Build/Win64/x64/UnrealEditor/Development/NullTide2/PerModuleInline.gen.cpp.obj.rsp` | 20 | Yes |
| 3 | `Intermediate/Build/Win64/x64/UnrealEditor/Development/NullTide2/PerModuleInline.gen.cpp.sarif` | 0 | Yes |
| 3 | `Intermediate/Build/Win64/x64/UnrealEditor/Development/NullTide2/SingleFile/InventoryItemFragment.h.cpp` | 2 | Yes |
| 3 | `Intermediate/Build/Win64/x64/UnrealEditor/Development/NullTide2/SingleFile/InventoryItemFragment.h.lc.rsp` | 12 | Yes |
| 3 | `Intermediate/Build/Win64/x64/UnrealEditor/Development/NullTide2/SingleFile/InventoryItemFragment.h.lcpt.dep.json` | 9 | Yes |
| 3 | `Intermediate/Build/Win64/x64/UnrealEditor/Development/NullTide2/SingleFile/InventoryItemFragment.h.lcpt.obj` | 29 | Yes |
| 3 | `Intermediate/Build/Win64/x64/UnrealEditor/Development/NullTide2/SingleFile/InventoryItemFragment.h.rsp` | 12 | Yes |
| 3 | `Intermediate/Build/Win64/x64/UnrealEditor/Development/NullTide2/UnrealEditor-NullTide2.dll.rsp` | 20 | Yes |
| 3 | `Intermediate/Build/Win64/x64/UnrealEditor/Development/NullTide2/UnrealEditor-NullTide2.dll.rsp.old` | 20 | Yes |
| 3 | `Intermediate/Build/Win64/x64/UnrealEditor/Development/NullTide2/UnrealEditor-NullTide2.exp` | 4 | Yes |
| 3 | `Intermediate/Build/Win64/x64/UnrealEditor/Development/NullTide2/UnrealEditor-NullTide2.lib` | 28 | Yes |
| 3 | `Intermediate/Build/Win64/x64/UnrealEditor/Development/NullTide2/UnrealEditor-NullTide2.lib.rsp` | 18 | Yes |
| 3 | `Intermediate/Build/Win64/x64/UnrealEditor/Development/NullTide2/UnrealEditor-NullTide2.lib.rsp.old` | 18 | Yes |
| 3 | `Intermediate/Build/XmlConfigCache.bin` | 1 | No |
| 3 | `Intermediate/CachedAssetRegistry/CachedAssetRegistry_0_DB4E69BF45B51859C67763A3D2AD4D8C.bin` | 2 | No |
| 3 | `Intermediate/ProjectFiles/cpp.hint` | 1 | No |
| 3 | `Intermediate/ProjectFiles/FileHashCache.bin` | 7 | No |
| 3 | `Intermediate/ProjectFiles/NullTide2.vcxproj` | 152 | Yes |
| 3 | `Intermediate/ProjectFiles/NullTide2.vcxproj.filters` | 26 | Yes |
| 3 | `Intermediate/ProjectFiles/NullTide2.vcxproj.user` | 6 | Yes |
| 3 | `Intermediate/TargetInfo.json` | 4 | No |
| 3 | `NullTide2.code-workspace` | 134 | Yes |
| 3 | `NullTide2.sln` | 2 | Yes |
| 3 | `NullTide2.slnx` | 1 | Yes |
| 3 | `Saved/Config/WindowsEditor/EditorPerProjectUserSettings.ini` | 12 | No |
| 3 | `Saved/ShaderDebugInfo/PCD3D_SM6/WorldGridMaterial_67bacdd81a65e5e9/Default/DDCKey-Editor.txt` | 1 | No |
| 4 | `.git/FETCH_HEAD` | 1 | No |
| 4 | `.git/index` | 14 | No |
| 4 | `INVENTORY_AUDIT.md` | 10 | No |
| 4 | `INVENTORY_BASELINE_FIX.md` | 13 | No |
| 4 | `Saved/Crashes/UECC-Windows-1AD279C4431AEDA6622C409443C933B9_0000/CrashContext.runtime-xml` | 1 | No |
| 4 | `Saved/Crashes/UECC-Windows-1AD279C4431AEDA6622C409443C933B9_0000/NullTide2.log` | 541 | Yes |
| 4 | `Saved/Crashes/UECC-Windows-1AD279C4431AEDA6622C409443C933B9_0000/UEMinidump.dmp` | 0 + UTF-16 detected | No |
| 4 | `Saved/Crashes/UECC-Windows-1AD279C4431AEDA6622C409443C933B9_0001/CrashContext.runtime-xml` | 1 | No |
| 4 | `Saved/Crashes/UECC-Windows-1AD279C4431AEDA6622C409443C933B9_0001/NullTide2.log` | 541 | Yes |
| 4 | `Saved/Crashes/UECC-Windows-1AD279C4431AEDA6622C409443C933B9_0001/UEMinidump.dmp` | 0 + UTF-16 detected | No |
| 4 | `Saved/Logs/NullTide2_2-backup-2026.09.13-16.23.52.log` | 73 | Yes |
| 4 | `Saved/Logs/NullTide2_2.log` | 65 | Yes |
| 4 | `Saved/Logs/NullTide2-backup-2026.09.13-15.20.03.log` | 65 | Yes |
| 4 | `Saved/Logs/NullTide2-backup-2026.09.13-17.11.02.log` | 69 | Yes |
| 4 | `Saved/Logs/NullTide2-backup-2026.09.14-12.48.31.log` | 65 | Yes |
| 4 | `Saved/Logs/NullTide2-backup-2026.09.14-13.26.48.log` | 75 | Yes |
| 4 | `Saved/Logs/NullTide2.log` | 88 | Yes |
| 5 | `.git/config` | 1 | No |
| 5 | `Saved/Autosaves/Game/LevelPrototyping/InventorySystem/BP_PickUpItem_Auto2.uasset` | 1 | No |
| 5 | `Saved/Autosaves/Game/LevelPrototyping/InventorySystem/InventoryManagerComponent_Auto2.uasset` | 1 | No |
| 5 | `Saved/Autosaves/Game/LevelPrototyping/InventorySystem/UI/Widgets/W_Inventory_Auto2.uasset` | 1 | No |
| 5 | `Saved/Autosaves/Game/LevelPrototyping/InventorySystem/UI/Widgets/W_InventorySlot_Auto2.uasset` | 1 | No |
| 5 | `Saved/SourceControl/UncontrolledChangelists.json` | 90 | No |

UTF-16LE also matched .vs/NullTide2/v17/.suo, Binaries/Win64/UnrealEditor-NullTide2.dll, Intermediate/Build/BuildRules/NullTide2ModuleRules.dll and the module's ItemDefinition.cpp.obj, NullTide2.cpp.obj and Module.NullTide2.gen.cpp.obj; all are category 3 already listed above. No source asset UTF-16-only match was found. Raw Git-object scan produced no literal matches; compressed history is outside semantic coverage.


