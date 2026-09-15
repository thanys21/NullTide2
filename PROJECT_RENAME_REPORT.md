# Project rename final report

Date: 2026-09-15
Project: C:/Users/loith/Documents/Unreal Projects/NullTide
Rename: NullTide2 -> NullTide

## Result

No unexpected build/runtime references were found in the requested authored-file scope. No source, runtime configuration or Blueprint fixes were required. The compatibility redirect remains intact. Only this report was created.

## Search scope and method

Case-insensitive searches for NullTide2, including uppercase API macro spellings, covered Source, Config, NullTide.uproject, project-local .codex, all .vscode files including ignored files/response files, root workspace/solution files, and project text documentation. Historical counts below are literal occurrence counts, not line counts.

Relevant live Unreal metadata was checked through MCP for the eight assets listed below using get_asset_tags and get_dependencies. A supplemental search of generated Intermediate/Binaries JSON, module manifests, target receipts and project-file formats found no matching contents. This is not a binary-wide or compressed-history scan.

## EXPECTED — preserve

| Location | Occurrences | Reason |
|---|---:|---|
| Config/DefaultEngine.ini:5 | 1 | Native package compatibility OldName |
| INVENTORY_AUDIT.md | 10 | Historical pre-rename architecture audit |
| INVENTORY_BASELINE_FIX.md | 13 | Historical baseline paths, API names and build command |
| PROJECT_RENAME_PLAN.md | 291 | Rename plan and historical occurrence inventory |
| PROJECT_RENAME_REPORT.md | Not counted | This report intentionally documents the old name |

Required configuration, unchanged:

```ini
[CoreRedirects]
+PackageRedirects=(OldName="/Script/NullTide2",NewName="/Script/NullTide")
```

Do not replace the OldName with the new name or remove this compatibility mapping. No gameplay classes were renamed, and no redundant class redirects were added.

## GENERATED — obsolete files safe to regenerate/delete

| File | Old-name text occurrences | Handling |
|---|---:|---|
| NullTide2.code-workspace | 134 | Obsolete generated build/launch paths; use NullTide.code-workspace |
| NullTide2.sln | 2 | Obsolete generated Visual Studio solution |
| NullTide2.slnx | 1 | Obsolete generated solution |
| Automation_NullTide2.sln | 0 | Old project name in filename only |
| Automation_NullTide2.slnx | 0 | Old project name in filename only |

These are generated leftovers, not authoritative build inputs. They were left untouched because the request authorizes fixing unexpected build/runtime references, not deleting generated history. Regenerate before using a Visual Studio/automation solution; do not launch the obsolete workspace.

Current NullTide.code-workspace and the entire .vscode tree contain no NullTide2 matches. The current VS Code generation therefore uses the renamed project rather than the obsolete root. Supplemental generated-file searches found no old-name contents in the selected Intermediate/Binaries formats.

## UNEXPECTED — none found

| Area | Result |
|---|---|
| Source | No old module, target, include or API macro matches |
| Config | Only the expected package redirect OldName |
| NullTide.uproject | No old-name matches |
| .codex | No old-name matches; MCP endpoint remains localhost:8000/mcp |
| .vscode | No old-name matches, including generated compile commands and response files |
| NullTide.code-workspace | No old-name matches |
| Current inventory asset metadata | No old-name tag values or dependencies returned |

No blanket substitutions were performed. Existing template settings unrelated to NullTide2 were outside this cleanup.

## Live Unreal MCP metadata validation

| Asset | Resolved parent | Tags containing old name | Old-name dependencies |
|---|---|---|---|
| Item_Wood | /Script/NullTide.ItemDefinition | None | None |
| Item_Sword | /Script/NullTide.ItemDefinition | None | None |
| InventoryManagerComponent | /Script/Engine.ActorComponent | None | None |
| BP_PickUpItem | /Script/Engine.Actor | None | None |
| W_InventorySlot | /Script/UMG.UserWidget | None | None |
| W_Inventory | /Script/UMG.UserWidget | None | None |
| BP_TopDownCharacter | /Script/Engine.Character | None | None |
| BP_TopDownController | /Script/Engine.PlayerController | None | None |

Item assets live under /Game/LevelPrototyping/InventorySystem/Items; inventory and pickup assets under /Game/LevelPrototyping/InventorySystem; widgets under its UI/Widgets directory; character/controller under /Game/TopDown/Blueprints.

Attempting get_referencers on /Script/NullTide2 returned "Asset does not exist: /Script/NullTide2". This tool requires an asset and cannot establish a global absence of old script-package references. The table instead reports successful per-asset metadata/dependency queries. No claim is made that every serialized byte or every asset outside this set has been migrated; redirects remain necessary for compatible old serialized content.

The preceding validation compiled these eight Blueprints successfully with warnings_as_errors=true. This final-search pass did not recompile or save them. The preceding clean native build of NullTideEditor Win64 Development also succeeded. Those are prior validation results, not builds performed for this report.

## Changes made

- Created PROJECT_RENAME_REPORT.md.
- No unexpected references required correction.
- Preserved CoreRedirect compatibility, historical documents and generated leftovers.
- No gameplay architecture, source, Blueprint assets or Codex/VS Code settings were modified.

