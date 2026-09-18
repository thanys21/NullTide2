# Inventory V2 M8 ¡ª Legacy Interactable cleanup

M8 is complete. Retired 17 proven-safe prototype packages; retained four required assets at their existing paths. No gameplay source, production Blueprint, map, inventory authority, UI, fragment behavior or source layout changed. No later milestone started. Changes are uncommitted for review.

## Baseline and audit

Baseline commit: 64f7541 (M7 complete). Working tree was clean before M8. SHA256 captured for all 280 Content .uasset/.umap files and all 38 Source files. All 21 Interactable files were backed up and hash-verified before deletion.

The prototype area contained four actor Blueprints, nine static meshes, one Niagara system, one material instance, two materials and four redirectors. It contained no interfaces, components, widgets, enums, maps, animations, DataAssets or DataTables. The complete pre-deletion inventory, parents, interfaces, direct referencers, dependencies, runtime roles, classifications and required actions are recorded in [M8_PROGRESS.md](M8_PROGRESS.md), checkpoint 2, and the exact JSON evidence below.

Inspected all 207 ordinary packages among the original 210 browseable registry assets, including dependency lists and Blueprint parent/default tags. Three external-object metadata assets reject AssetTools existence checks; global package referencer queries still include World Partition external actors. The only project world is /Game/TopDown/Lvl_TopDown. It has one Door and two Sword placements, and no JumpPad or WobbleTarget placements. Defaults, component templates, interface use and graph references were inspected. No project DataTable or PrimaryDataAsset was found, and config primary-asset scanning contains no explicit candidate paths or labels.

MCP AssetRegistryDependencyOptions includes hard and soft package references and game/editor references by default, verified against local engine headers. Its wrapper does not expose searchable-name/management reference categories. Separate Blueprint parent/default, graph, source/config string, map and data-asset checks supplement the package audit. M7's complete 20-Blueprint/165-graph audit remains applicable because all 280 M8 baseline content hashes exactly match M7's final hashes; candidate literal references were searched there, with a fresh targeted 10-Blueprint/31-graph interaction audit and enhanced-input node inspection.

All DELETE assets belong to a closed, unused group. No required incoming reference from outside that group existed. Root assets were deleted first; every subsequent asset was rechecked for zero remaining global package referencers immediately before deletion.

## Production interaction remains intact

The preserved production path is BP_TopDownCharacter -> PlayerInteractionComponent -> BPI_Interactable -> interacting actor, with E_InteractionType and WB_interactionWidget providing PRESS/HOLD prompts and progress.

- PlayerInteractionComponent discovers interface actors through character capsule overlaps, maintains its interaction range, creates one widget component and handles enhanced-input press/hold events.
- BPI_Interactable defines Interact, GetInteractionType and GetHoldDuration.
- E_InteractionType supplies Press/Hold; WB_interactionWidget displays the mapped interaction key and hold progress.
- BP_TopDownCharacter is the production component owner/caller.
- BP_PickUpItem remains the automatic, result-aware inventory transaction overlap path. It uses the retained shared sword mesh and does not depend on the prototype Sword actor.

The four protected production interaction assets are byte-identical to baseline. No competing prototype interface/component/helper framework was found in Interactable.

BP_Door and BP_Sword already implement the production BPI_Interactable. Their required behavior therefore needs no migration or graph rewrite. BP_Sword is a world hold-interaction/audio/destruction actor, with 1s and 3s map-instance defaults; it has no ItemDefinition, equipment or inventory award semantics and is distinct from Item_Sword and the canonical Weapon fragment. The Door has existing press-driven open/close timeline behavior. Both remain useful and placed.

## Final classification and changed files

Classification: 4 KEEP, 0 MIGRATE, 17 DELETE, 0 BLOCKED. Exact full reference matrix was written before the first deletion in M8_PROGRESS.md. All paths below are relative to /Game/LevelPrototyping/Interactable.

| Asset | Type | Decision | Required role / retirement reason |
| --- | --- | --- | --- |
| Box_5F17EA8B | ObjectRedirector | DELETE | Zero required referencers; obsolete primitive redirector |
| Cone_D585226A | ObjectRedirector | DELETE | Zero required referencers; obsolete primitive redirector |
| Cylinder_D3A4EA1B | ObjectRedirector | DELETE | Zero required referencers; obsolete primitive redirector |
| Door/Assets/SM_Door | StaticMesh | KEEP | Retained Door visual |
| Door/BP_Door | BP_Door_C | KEEP | One active map actor; production press interaction |
| JumpPad/Assets/Materials/M_GradientGlow | Material | DELETE | Unused primitive or dependency exclusively owned by retired prototypes |
| JumpPad/Assets/Materials/M_SimpleGlow | Material | DELETE | Unused primitive or dependency exclusively owned by retired prototypes |
| JumpPad/Assets/Materials/MI_GlowNT | MaterialInstanceConstant | DELETE | Unused primitive or dependency exclusively owned by retired prototypes |
| JumpPad/Assets/Meshes/SM_CircularBand | StaticMesh | DELETE | Unused primitive or dependency exclusively owned by retired prototypes |
| JumpPad/Assets/Meshes/SM_CircularGlow | StaticMesh | DELETE | Unused primitive or dependency exclusively owned by retired prototypes |
| JumpPad/Assets/NS_JumpPad | NiagaraSystem | DELETE | Unused primitive or dependency exclusively owned by retired prototypes |
| JumpPad/BP_JumpPad | BP_JumpPad_C | DELETE | Unplaced standalone overlap/launch prototype |
| SM_Box | StaticMesh | DELETE | Unused primitive or dependency exclusively owned by retired prototypes |
| SM_Cone | StaticMesh | DELETE | Unused primitive or dependency exclusively owned by retired prototypes |
| SM_Cylinder | StaticMesh | DELETE | Unused primitive or dependency exclusively owned by retired prototypes |
| SM_Sphere | StaticMesh | DELETE | Unused primitive or dependency exclusively owned by retired prototypes |
| Sphere_8D4711EB | ObjectRedirector | DELETE | Zero required referencers; obsolete primitive redirector |
| Sword/Assets/SM_Sword_E8D4799D | StaticMesh | KEEP | Shared by retained Sword and production BP_PickUpItem |
| Sword/BP_Sword | BP_Sword_C | KEEP | Two active map actors; production hold interaction |
| Target/Assets/SM_TargetBaseMesh | StaticMesh | DELETE | Unused primitive or dependency exclusively owned by retired prototypes |
| Target/BP_WobbleTarget | BP_WobbleTarget_C | DELETE | Unplaced physics-constraint learning actor |

Deleted files: the 17 DELETE rows above, each corresponding to Content/LevelPrototyping/Interactable/{asset-row}.uasset. Thirteen nonredirector assets were deleted through MCP AssetTools.delete: two Blueprints, seven meshes, one Niagara system, one material instance and two materials. Four redirectors were removed by the installed Unreal ResavePackages commandlet with only those four explicit package arguments. No .uasset bytes were edited or manually deleted.

The first zero-referencer Box redirector MCP delete returned success but left its package on disk. The batch stopped and the persistence was reviewed. Local engine commandlet scope and final referencer checks were inspected before running:

    UnrealEditor-Cmd.exe NullTide.uproject -run=ResavePackages -fixupredirects -projectonly -unattended -nop4 -NullRHI -stdout
      -PACKAGE=/Game/LevelPrototyping/Interactable/Box_5F17EA8B
      -PACKAGE=/Game/LevelPrototyping/Interactable/Cone_D585226A
      -PACKAGE=/Game/LevelPrototyping/Interactable/Cylinder_D3A4EA1B
      -PACKAGE=/Game/LevelPrototyping/Interactable/Sphere_8D4711EB

The commandlet rechecked zero referencers and removed exactly the four redirector files, with no other assets resaved. This uses Unreal's documented [redirector fixup workflow](https://dev.epicgames.com/documentation/unreal-engine/asset-redirectors-in-unreal-engine), scoped to the explicit packages. The cleaned folder contains four KEEP assets and zero redirectors; no moves or new redirectors were introduced.

Added documentation: INVENTORY_V2_M8_REPORT.md and M8_PROGRESS.md. No existing retained .uasset/.umap, Source, Config, project descriptor or project-local configuration file changed.

## Validation

| Check | Result |
| --- | --- |
| NullTideEditor Win64 Development | PASS; installed UE5.8.1, target up to date, zero build actions, 1.02s, exit0 |
| Permanent native tests | PASS, 43/43; zero failures/skips/test errors/test warnings; unchanged M7 test names |
| Blueprint compile, warnings_as_errors=true | PASS, all 13 listed below; parents/interfaces resolve |
| Initial PIE component/state check | One inventory component, one interaction component; NativeReady, legacy mode false; empty inventory, Revision1 |
| Door production interaction | PRESS E prompt; repeated open/close alternates true/false/true/false; existing timeline behavior preserved |
| Prototype Sword production interaction | HOLD E prompt; 3s target survives canceled partial hold then completes; 1s progress about0.667 then completes; both PIE actors destroy and inventory remains empty/Revision1 |
| Inventory Wood/Sword pickups | One instance per pickup, unique GUIDs, order [Wood,Sword], matching compatibility projection; Revision2 then3 |
| Current inventory UI | Open before pickups; remains in viewport and refreshes to exactly two W_InventorySlot children without reopening |
| Public native snapshot | Exactly two instances, each Outer is the existing manager component; no duplicate inventory component |
| Map reload after PIE | PASS; one Door, two prototype Swords and both inventory pickup actors resolve again |
| Final package dependencies | PASS; 190 surviving ordinary packages checked, zero dependencies on the 17 retired packages; registry193 |
| Windows cook, Lvl_TopDown | PASS, exit0, 595 cooked packages/0 remaining, 0 errors/1 existing MCP notice; unchanged packaging/render settings |
| Final file scope after editor closure | PASS; Content280->263, exactly17 deletions, 0 modified/added/moved; all38 Source hashes unchanged |

Native test distribution remains M1:9, M2:4, M3:2, M4:10, M6:18. No permanent test count change.

Compiled the original eight inventory Blueprints: InventoryManagerComponent, BP_PickUpItem, Item_Wood, Item_Sword, W_Inventory, W_InventorySlot, BP_TopDownCharacter, BP_TopDownController; plus PlayerInteractionComponent, BPI_Interactable, WB_interactionWidget, BP_Door and BP_Sword. Compilation did not save any assets. Item_Wood/Item_Sword retain /Script/NullTide.ItemDefinition, their original defaults and M6 fragment data; all assets are byte-identical to baseline.

PIE was driven through MCP scene transforms, Slate and the Unreal enhanced-input console, exercising the existing production graphs. The test pawn's runtime movement speed was temporarily set to zero to prevent click-to-move drift; all runtime adjustments and destroyed actors were discarded by StopPIE. No map/class default was changed. Public GetItemsSnapshot, reflected instance GUID queries and the existing widget's child count were read through MCP/editor tooling.

Observed native IDs were AA2FBED1-4A15-4A70-7476-D0A74C9A5484 (Wood) and 26F04077-4D60-B150-4474-63A0C2CD9690 (Sword), both owned by the character's existing InventoryManagerComponent. The native store remains authoritative; no inventory or UI boundary changed.

No cleanup-related missing package, unresolved class/interface, missing placed actor or Blueprint compile error was found. Protected production interaction assets, original eight inventory Blueprints, retained four assets, map/external-actor files and all other surviving Content packages match baseline SHA256. Only the approved retirement set is absent.

## Warnings, risks and recovery

- The existing PlayerInteractionComponent GetActiveInteractable graph indexes LastIndex on an empty InteractableInRange array. PIE emits an existing index-1 ScriptWarning when leaving range or destroying the last target. Its production graph/hash is unchanged. Stable-range repeated Door interactions and both Sword completions passed; M8 does not fix this unrelated production edge case.
- The build reports installed MSVC14.51.36257 newer than preferred14.50.35717. No source/compiler configuration change was made.
- Cook has one existing MCP startup notice. Scoped redirector fixup has that notice plus four expected unknown revision-control-state warnings under -nop4; exit0 and exact scope were verified.
- Diagnostic MCP calls during PIE can log editor-world guard/schema failures. An initial read-only Python UI probe used an unavailable binding/wrong UI path; the corrected public snapshot/existing-widget probe passed. These diagnostic errors did not affect gameplay assets, compilation or cook.
- Required Door/Sword assets intentionally remain under Interactable. Production BP_PickUpItem still references the retained shared sword mesh there. No retired prototype framework is required; removing every reference to this folder would incorrectly remove a useful production visual.
- Registry package-reference checks cannot prove arbitrary dynamically constructed asset paths in uninspected future code. Current source/config/graph/default/map/data checks found no runtime reference into the retired group; preserve this evidence and backups during review.
- The first owned audit editor stalled on normal MCP close. Its owned PID/launch and disk scope were verified before terminating only that validation process, with no authored unsaved data. Fresh validation editor subsequently completed all checks and closed normally after PIE stopped. No editor remains from this run.

## Evidence and rollback

Evidence is under Saved/Validation/InventoryV2M8 (generated/ignored; retain locally):

- assets-before.json, assets-after.json, source-before.json, source-after.json, final-scope-result.json.
- prototype-registry-before.json, classification-before.json, interaction-blueprints-before.json, prototype-default-values-before.json, map-placements-before.json.
- all-registry-before.json, dependencies-after.json, references-after-result.json, prototype-registry-after-delete.json, map-reload-after.json.
- deletions.json/deletions.jsonl and deletion-attempts.jsonl; fixup-redirectors.log and build-editor.log.
- blueprint-compiles.json, native-tests.json and cook-windows.log/cook-result.json.
- pie-initial.json, pie-door-repeated-final.json, pie-sword3-hold-final.json, pie-sword1-hold-final.json, pie-inventory-pickups.json, pie-native-instances.json, pie-native-ui-state-verified.json and the empty/two-item UI screenshots.
- editor-fresh.log and BaselineBackup/Content/LevelPrototyping/Interactable, containing all21 hash-verified baseline prototype packages.

For rollback, close Unreal Editor and restore the complete 17-file retirement set from baseline commit64f7541 through source control, or the verified baseline backup. Restore the group coherently, then reopen the project, validate references, compile the relevant Blueprints and rerun regression/cook before continuing. No class/module/source migration or redirect change needs reversal. Do not discard the backups or validation logs before review.

M8 is complete and stopped. No pending destructive operation and no later milestone work.

