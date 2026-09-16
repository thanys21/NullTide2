# Inventory V2 M4 report

Status: **complete and validated** on 2026-09-16. M5 has not begun.

Baseline commit: `7b7d0fd102fd678ddce196c9b67edfa5b0417f27` (clean repository at start).

## Result

The existing Blueprint InventoryManagerComponent remains the character's single inventory component and retains its `/Script/NullTide.InventoryComponent` parent. At BeginPlay, it atomically imports its serialized InventoryItems seed and switches to native Items authority. InventoryItems becomes a native-derived compatibility projection for the unchanged inventory widgets. No second active inventory store was introduced.

The old InventoryItems variable, AddItem(ItemDefinitions) signature, compatibility library signatures, pickup graph and widget behavior are preserved. No folders, assets or gameplay classes were moved, renamed or removed.

## Initialization and transaction contract

- The new default-only `bImportLegacyInventoryOnBeginPlay` setting is enabled on the manager and its existing character component template through Unreal MCP.
- Serialized pre-play defaults remain `bLegacyInventoryMode=true`, `InventoryItems=[]`, `Revision=0`. Native BeginPlay imports before calling Super::BeginPlay, so Blueprint BeginPlay sees the completed cutover rather than a writable legacy seed.
- Reflected initialization states are NotRequired, PendingLegacyImport, NativeReady and Failed. PendingLegacyImport is reported by the getter before an opted-in/legacy instance initializes; NativeReady/Failed and the last initialization result are retained explicitly.
- Import requires an untouched native store/revision and a reflected InventoryItems class-reference array compatible with UItemDefinition. It copies the seed once and validates every definition/class/data entry before creating any staged item.
- Null, abstract/deprecated/replaced classes, incompatible classes and invalid definition data fail without changing Items, Revision, the seed or legacy mode. Failure sets terminal Failed state and retains the error; production mutations remain blocked. Repair requires a fresh instance rather than retrying a partially used instance.
- Staged instances use the component as Outer, strong staging references during import, and reflected strong references in private Items after commit. Entry order and duplicate definitions are retained; every entry receives a distinct valid FGuid.
- Commit publishes native Items, rebuilds InventoryItems, disables runtime legacy mode and sets NativeReady/Success before incrementing Revision and notifying once. Empty seed initialization is also one successful commit: Revision 1 and one event.
- Repeated successful initialization returns Success without reading the projection, replacing instances, changing Revision or notifying again.
- Native add/remove validate before mutation and rebuild the projection from native Items before Revision/event publication. Failed transactions leave authoritative membership, projection, Revision and event count unchanged. Reentrant initialization/mutation during notification returns Busy.
- After cutover, compatibility add calls native TryAddDefinition and compatibility queries derive definitions from native snapshots. They never import or append to the projection. Pending/failed opted-in legacy instances reject writes. Explicit legacy rollback fixtures retain the old array path.

## Files added

All source paths below are relative to `Source/NullTide/Content/InteractionSystem/Inventory/`.

- `LegacyInventoryStorage.h`
- `LegacyInventoryStorage.cpp` — internal reflected legacy-array schema/read/projection-write adapter.
- `Tests/InventoryCutoverTests.cpp` — ten M4 automation tests.

Documentation added: `INVENTORY_V2_M4_REPORT.md` and `M4_PROGRESS.md`.

## Files modified

Within the same inventory source directory:

- `InventoryComponent.h` — initialization/query API, auto-import option, state and projection support.
- `InventoryComponent.cpp` — BeginPlay cutover, atomic staged import, idempotence, native transaction projection and authority guards.
- `InventoryTypes.h` — EInventoryInitializationState; existing operation result values/signatures preserved.
- `LegacyInventoryCompatibilityLibrary.h` — documentation updated for mode-aware compatibility dispatch; signatures retained.
- `LegacyInventoryCompatibilityLibrary.cpp` — completed-cutover native dispatch/query and pending/failed write blocking.
- `Tests/InventoryComponentTestTypes.h` — transient cutover/definition fixtures and notification observation fields.
- `Tests/InventoryComponentTests.cpp` — callback observations and explicit legacy rollback configuration on the transient M3 Blueprint fixture; existing legacy assertions retained.

Blueprint assets modified through Unreal MCP/editor tooling only:

| Asset file | Intended change |
| --- | --- |
| `Content/LevelPrototyping/InventorySystem/InventoryManagerComponent.uasset` | Enable controlled BeginPlay import; preserve AddItem signature and route its implementation through the existing result-aware compatibility wrapper. |
| `Content/TopDown/Blueprints/BP_TopDownCharacter.uasset` | Enable controlled import on the existing inventory component template; preserve that component instance and all previous defaults. |

No production UI or pickup asset was edited. Existing M2 test source was unchanged.

## Public Blueprint API

New on UInventoryComponent:

| API | Exposure | Meaning |
| --- | --- | --- |
| `InitializeFromLegacyInventory()` | BlueprintCallable | One-time import returning FInventoryOperationResult; subsequent success is an idempotent no-op. |
| `GetInitializationState()` | BlueprintPure | Initialization/cutover status. |
| `GetLastInitializationResult()` | BlueprintPure | Retained initialization result code. |
| `IsNativeAuthorityActive()` | BlueprintPure | Whether native transactions are currently enabled. |
| `bImportLegacyInventoryOnBeginPlay` | EditDefaultsOnly, BlueprintReadOnly | Controlled production initialization opt-in. |
| `InitializationState`, `LastInitializationResult` | Transient, VisibleInstanceOnly, BlueprintReadOnly | Read-only runtime diagnostics. |
| `EInventoryInitializationState` | BlueprintType enum | NotRequired, PendingLegacyImport, NativeReady, Failed. |

Existing TryAddDefinition, TryRemoveItem, snapshots/queries, Revision, OnInventoryChanged, legacy-mode getter and operation result APIs remain available. Compatibility wrapper names and parameters remain unchanged. The compatibility wrapper requires completed NativeReady cutover before routing a native receiver, preserving the M2 rejection contract for plain native components without a legacy boundary; standalone M1 native transactions remain supported directly.

## Blueprint graph changes

Only the manager AddItem function graph changed. Its previous direct Array Add became:

```lisp
(fn AddItem (ItemDefinitions)
  (Inventory|Compatibility|TryAddDefinitionToLegacyInventory self ItemDefinitions))
```

Its existing void signature remains intact. In native mode, this invokes one native transaction; in explicit legacy rollback mode, the existing compatibility wrapper uses the old array path. Destructive pickup callers retain their result-aware success branch.

MCP graph exports confirm BP_PickUpItem EventGraph and W_Inventory EventGraph are exactly unchanged. All previously captured manager/template property values are unchanged; the only intended new default override is auto-import enabled.

## Build and native tests

Target: **NullTideEditor**, platform: **Win64**, configuration: **Development**. Engine: associated installed Unreal Engine **5.8.1**, `D:/game/Unreal Engine/UE_5.8`.

Initial native implementation build succeeded, including UHT and new source compilation. Final compatibility-fix build succeeded (4.56 seconds), followed by editor restart using the rebuilt module.

The initial test run found one regression: compatibility dispatch accepted a plain native component without completed legacy cutover. Added a NativeReady requirement in compatibility mutation/query dispatch; did not alter the existing M2 rejection test. Final full run: **25 passed, 0 failed, 0 skipped** (9.401 seconds).

| Suite | Passed |
| --- | --- |
| M1 native foundation | 9/9 |
| M2 legacy transaction/query boundary | 4/4 |
| M3 parent/single-component/legacy rollback compatibility | 2/2 |
| M4 native cutover | 10/10 |

M4 coverage: duplicate/order/identity/Outer preservation; empty cutover; null seed atomic terminal failure; invalid data/abstract seed atomic failure; idempotence and projection-tamper rejection; native add/remove projection before notification and failure invariants; native compatibility dispatch/pending-write blocking; reentrant initialization and mutation Busy; missing seed schema rejection; actual Blueprint legacy AddItem routing Wood/Sword through native authority.

Warnings:

- Existing build notice: installed MSVC 14.51.36257 is newer than preferred 14.50.35717.
- Existing include-order upgrade notice: target retains Unreal5_6 include order. No unrelated target settings changed.
- One final successful M4 test captured an engine/background HTTP connectivity probe timeout to `https://www.google.com/generate_204`; no test errors occurred.
- All eight Blueprint compilations passed with warnings treated as errors; no Blueprint compile warnings/errors were accepted.
- MCP runtime actor movement emitted editor-world lookup diagnostics while successfully moving the PIE pawn. This is tooling behavior; no unrelated tooling/gameplay changes were made.

## Asset validation

All original eight assets loaded and compiled with `warnings_as_errors=true`, including after the final rebuilt editor restart:

- InventoryManagerComponent
- BP_PickUpItem
- Item_Wood
- Item_Sword
- W_Inventory
- W_InventorySlot
- BP_TopDownCharacter
- BP_TopDownController

Manager parent remains `/Script/NullTide.InventoryComponent`. Item_Wood and Item_Sword remain parented to `/Script/NullTide.ItemDefinition`; both asset hashes/defaults are unchanged. The character's existing component path is preserved; native-type component enumeration returns exactly one inventory component before/during play.

Compared all 280 Content .uasset/.umap hashes to the baseline: **two intended changes, 278 unchanged, zero added/deleted assets/maps**. No unrelated .uasset is changed on disk. One normal editor close initially saved an unchanged compiler-generated W_Inventory version; read-only Unreal DiffAssets exports matched authored data after temporary package-path/GUID normalization, and a hash-guarded, scoped source-control restore while the editor was closed returned it to baseline. No .uasset bytes were manually patched.

## PIE regression

World: `/Game/TopDown/UEDPIE_0_Lvl_TopDown`. Spawned away from pickups, then moved the pawn into Wood and Sword overlaps through MCP.

| Checkpoint | InventoryItems projection | Revision | Legacy mode | State |
| --- | --- | --- | --- | --- |
| Empty start after import | [] | 1 | false | NativeReady |
| Wood overlap | [Wood] | 2 | false | NativeReady |
| Move away | [Wood] | 2 | false | NativeReady |
| Sword overlap | [Wood, Sword] | 3 | false | NativeReady |

Wood pickup disappeared after one award; Sword remained until its own overlap, then disappeared after one award. No extra award occurred while moving away/opening the UI.

Read-only engine object dump confirmed exactly two entries in private native Items. MCP read each instance's identity/definition:

| Native insertion index | InstanceId | Definition |
| --- | --- | --- |
| 0 | `7D285C51-4E58-4C1E-A734-EBAB7CE28BF1` | Item_Wood |
| 1 | `90E9391E-4F6F-8F54-AC61-20B2D13FECD5` | Item_Sword |

Both object paths are children of the existing runtime InventoryManagerComponent (`ItemInstance_0`, `ItemInstance_1`), confirming component Outer. Exactly one runtime inventory component was enumerated. Native Items is authoritative; the legacy array is its matching definition projection.

Pressed I to open the unchanged W_Inventory. Runtime object enumeration found one W_Inventory and two W_InventorySlot instances; slot definition properties were Wood then Sword. Screenshot visually confirms both icons. PIE was stopped after inspection.

Duplicate/invalid seed, repeat initialization, failed native transaction and legacy AddItem-after-cutover checks used automation fixtures (including the actual manager Blueprint) rather than changing production seeds/assets for testing.

## Evidence and rollback

Generated evidence is preserved under `Saved/Validation/InventoryV2M4/`: build logs, both test runs, final Blueprint compilation results, before/after defaults/graphs, baseline/final asset hashes, PIE checkpoints/identities/object dumps and UI screenshot. Saved evidence is generated/ignored, not gameplay content.

Rollback point is the baseline commit above. Stop PIE/editor normally, preserve authored unsaved work, then restore the M4 source and two Blueprint assets coherently through source control and rebuild. Serialized legacy seeds/default legacy mode and the compatibility signatures remain available. Do not toggle authority on an already cut-over runtime instance or feed its projection back as a seed.

## Remaining risks and scope limits

- InventoryItems stays Blueprint-writable for compatibility. External direct writes can temporarily corrupt what old readers display, but cannot change native membership or be imported after cutover; the next successful transaction rebuilds the projection. M5 and later API cleanup must eliminate obsolete direct writers deliberately.
- Legacy UI still builds slots on construct and has no native event/snapshot migration; that remains M5.
- Invalid production seeds fail closed with a retained result and warning; automated fixtures cover the failure, and production item defaults were not modified.
- No save/load, replication, stacking, quantity, equipment, transfer or drop-spawn work was introduced. Runtime initialization/state is not persistence design.
- No unrelated systems were changed. Changes are ready for review/commit; this task did not create a commit.
