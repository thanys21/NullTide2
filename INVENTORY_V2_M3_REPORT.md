# Inventory V2 M3 report

Status: **M3 complete and validated**, 2026-09-16. InventoryManagerComponent now directly inherits /Script/NullTide.InventoryComponent. Legacy InventoryItems remains the sole authoritative gameplay store. **M4 has not begun.**

Baseline commit: 47ccc24a6ce07dcd8d61005a33c76362da1acf72; the repository was clean before M3.

## References inspected before editing

MCP asset-registry inspection found three direct manager referencers:

- /Game/LevelPrototyping/InventorySystem/BP_PickUpItem
- /Game/LevelPrototyping/InventorySystem/UI/Widgets/W_Inventory
- /Game/TopDown/Blueprints/BP_TopDownCharacter

Indirect referencers include BP_TopDownController, which references W_Inventory; BP_TopDownGameMode, which references the character/controller; and Lvl_TopDown, which references the game mode. Two placed pickup packages reference BP_PickUpItem:

- /Game/__ExternalActors__/TopDown/Lvl_TopDown/D/VJ/M3RM8HR4BE1SRMFMYDNVSU
- /Game/__ExternalActors__/TopDown/Lvl_TopDown/E/8K/ESLR750XUODV4CZ61M4ZMD

Both package files exist. MCP cannot inspect these package-only paths as ordinary assets; the asset-registry inbound references and filesystem presence were verified. No additional manager subclass was found. Source/Config inspection found the existing M2 reflection boundary and test fixtures, with no native dependency on the production manager asset.

Before editing, BP_TopDownCharacter owned exactly one existing manager template. Its InventoryItems defaults and the manager CDO defaults were both empty. All 13 preexisting reflected defaults and relevant AddItem, pickup and inventory UI graph DSL were captured.

## Files modified

| File | Change |
| --- | --- |
| Source/NullTide/Content/InteractionSystem/Inventory/InventoryComponent.h | Added a defaults-only legacy-mode setting and read-only Blueprint getter. Existing APIs retained. |
| Source/NullTide/Content/InteractionSystem/Inventory/InventoryComponent.cpp | Native mutation precondition rejects add/remove while legacy mode is enabled. |
| Source/NullTide/Content/InteractionSystem/Inventory/Tests/InventoryComponentTests.cpp | Added two M3 native regression tests. |
| Content/LevelPrototyping/InventorySystem/InventoryManagerComponent.uasset | MCP reparent from ActorComponent to InventoryComponent; enabled legacy mode on class defaults. |
| Content/TopDown/Blueprints/BP_TopDownCharacter.uasset | Enabled legacy mode on the existing manager component template, retaining its slot and old defaults. |
| INVENTORY_V2_M3_REPORT.md | Created, then finalized this report. |
| M3_PROGRESS.md | Created and updated milestone checkpoints. |

No gameplay classes, APIs, folders or assets were renamed or moved. All intentional Blueprint changes, compilation and saves used Unreal MCP/editor tooling. No serialized Blueprint bytes were patched.

## Compatibility and Blueprint changes

- Manager parent is now **/Script/NullTide.InventoryComponent**.
- InventoryItems remains the original reflected class-reference array.
- AddItem(ItemDefinitions) retains its signature and unchanged graph: Array Add into InventoryItems.
- BP_PickUpItem retains the M2 result-aware compatibility transaction and success-only DestroyActor path.
- W_Inventory still constructs slots from legacy InventoryItems. W_InventorySlot retains its class-reference input.
- All 13 original manager defaults and all 13 original character component-template defaults match the captured baseline.
- Character component template retains the exact path ending BP_TopDownCharacter_C:InventoryManagerComponent_GEN_VARIABLE. There is exactly one component assignable to UInventoryComponent on the character CDO and in PIE.

Reparenting initially left the character template's new migration setting at the native default false. Recompiling did not propagate the manager CDO setting. MCP explicitly set true on that same existing template and saved the affected character asset. Reopening confirmed both settings persisted.

## Native compatibility API

Added BlueprintPure bool IsLegacyInventoryMode() const in Inventory|Compatibility.

Private reflected bLegacyInventoryMode is EditDefaultsOnly and BlueprintReadOnly. It defaults to false on standalone native components, preserving M1 behavior. The manager and its existing character template set it to true.

While true, native TryAddDefinition and TryRemoveItem return NotInitialized before changing Items, Revision or OnInventoryChanged. There is no runtime setter, legacy seed import, fallback store, native authority cutover or second attached component. Native queries remain queries of the empty native array; production callers continue using the legacy boundary.

## Build and native tests

**NullTideEditor Win64 Development: succeeded**, using the installed associated UE_5.8 engine.

The first build compiled the changed C++ and generated reflection code but could not link because the running editor locked UnrealEditor-NullTide.dll (LNK1104). After the user closed the editor normally, linking and metadata generation succeeded. No unrelated compiler error required a fix.

Existing notices: MSVC 14.51.36257 is newer than preferred 14.50.35717; target include order remains Unreal5_6. No toolchain or target settings were changed.

**15 native automation tests passed; 0 failed, 0 skipped, 0 test warnings/errors.** This includes all nine M1 tests, four M2 tests and:

- NullTide.Inventory.M3.LegacyModeRejectsNativeMutations: disabled native add/remove leave membership empty, revision zero and event count zero.
- NullTide.Inventory.M3.ReparentedBlueprintKeepsLegacyAuthority: loads the actual manager subclass; checks direct parent and legacy-mode default; invokes preserved AddItem(ItemDefinitions) with Wood/Sword on a transient instance; verifies one legacy append per call and no native membership or revision change.

Tests passed again after reopening the saved M3 assets.

## Blueprint validation

All eight compiled through MCP with warnings_as_errors=true, including after reopening:

| Blueprint | Result |
| --- | --- |
| InventoryManagerComponent | Pass; intended native parent, old variable and function preserved. |
| BP_PickUpItem | Pass; original M2 graph unchanged. |
| Item_Wood | Pass; parent /Script/NullTide.ItemDefinition; serialized asset unchanged. |
| Item_Sword | Pass; parent /Script/NullTide.ItemDefinition; serialized asset unchanged. |
| W_Inventory | Pass; legacy inventory reader unchanged. |
| W_InventorySlot | Pass; legacy class-reference input unchanged. |
| BP_TopDownCharacter | Pass; same single component and preserved original defaults. |
| BP_TopDownController | Pass; existing inventory input path retained. |

The captured AddItem, pickup and inventory-widget graph DSL match the final graphs exactly.

## PIE regression

Controlled tests ran in /Game/TopDown/Lvl_TopDown and passed again in a fresh session after reopening.

| Checkpoint | Observed result |
| --- | --- |
| Start | Legacy array empty; legacy mode true; native revision 0. |
| Wood overlap | Exactly one Wood entry; Wood pickup disappeared; Sword remained. |
| Sword overlap | Exactly two entries in insertion order [Wood, Sword]; both pickups gone. |
| Open inventory with I | One runtime W_Inventory and exactly two W_InventorySlot objects in its widget tree. Slot 0 references Wood; slot 1 references Sword. Two icons visible in the MCP screenshot. |
| Component count | Exactly one component assignable to UInventoryComponent, named InventoryManagerComponent. |
| Native state | Unfiltered read-only obj dump of that runtime component shows the two legacy entries, legacy mode true, revision 0 and no native Items entries. |

MCP ObjectTools cannot read the private non-Blueprint-visible Items property. The editor's read-only property dump was used instead: it enumerates every populated array element, so the absence of Items entries confirms the native array is empty. Native regression tests independently verify the disabled mutation path.

No legacy entries were imported. The only active production inventory store is InventoryItems. PIE is stopped at handoff.

## Asset scope and diagnostic recovery

Final SHA-256 comparison covers **280 Content .uasset/.umap files**. Exactly two differ: the intentionally edited manager and character assets. No assets were added/deleted. All other 278 files match the initial baseline, including pickup, both item definitions, both inventory widgets, controller and map.

The user's editor-close step saved compiler-only versions of BP_PickUpItem and W_Inventory. Read-only Unreal DiffAssets exports matched their committed authored data after normalizing temporary export package paths and generated GUIDs. Scoped source-control restoration returned those two files to the original committed copies; hash guards checked for intervening changes before restoration.

During file-lock cleanup, I invoked debug resetloaders through MCP Slate. The editor crashed with an access violation in CoreUObject after PIE had stopped and the intended M3 assets had been saved. The log was preserved. The editor was reopened, saved defaults were confirmed, all eight compilations and all 15 tests passed again, and the fresh PIE regression passed. No engine/plugin code was modified; this diagnostic should not be repeated.

Generated evidence is under Saved/Validation/InventoryV2M3: build log, before/after defaults, graph checks, Blueprint compilation results, native test results, PIE state/slot data, property dump, UI screenshot, content hash verification and the diagnostic crash log. These are local generated artifacts.

## Remaining limits and rollback

- Existing AddItem and direct legacy array writes retain their M2 compatibility limits; no validation redesign was introduced.
- M4 must handle both manager defaults and existing component-template migration settings at a separate stopped-editor checkpoint. No native import or native write path is enabled here.
- Blueprint compilation can mark caller packages dirty in memory. Only manager/character assets were intentionally saved; final on-disk scope is verified.
- MCP transform tooling emitted editor-world lookup errors during PIE despite successful runtime transforms and verified pickup results. PIE also reported the existing audio device sample-rate conversion warnings. No Blueprint runtime exception was found in the final regression.
- The loader-reset diagnostic crash was recovered and documented; its underlying engine/tooling cause remains uninvestigated within M3 scope.

Rollback checkpoint: restore the two M3 asset snapshots and three modified native files coherently from the baseline commit, then rebuild. Stop PIE/editor before rollback. There is no native inventory state to translate back.

**Stop point: M3 only. No M4 authority cutover, UI migration, replacement component, source move or asset move was performed.**
