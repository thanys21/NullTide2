# Inventory V2 M2 — Legacy Boundary and Pickup Success Handling

## Outcome

M2 implementation and gameplay validation are complete. The existing Blueprint `InventoryManagerComponent` remains an `ActorComponent`, and its `InventoryItems` class-reference array remains authoritative. No native `UInventoryComponent` was attached or activated as a second inventory store. M3 has not started.

The strict unchanged-file validation has one documented exception: after the interrupted run, the editor exit dialog saved compiler-generated changes in `W_Inventory` and `BP_TopDownCharacter`. Both files were already changed when work resumed. Read-only Unreal text exports found no gameplay graph, component, default, or reference changes after accounting for compiler-generated identifiers. They were preserved rather than overwritten.

## Files

### Added

- `Source/NullTide/Content/InteractionSystem/Inventory/LegacyInventoryCompatibilityLibrary.h`
- `Source/NullTide/Content/InteractionSystem/Inventory/LegacyInventoryCompatibilityLibrary.cpp`
- `Source/NullTide/Content/InteractionSystem/Inventory/Tests/LegacyInventoryCompatibilityTests.cpp`
- `INVENTORY_V2_M2_REPORT.md`

### Intentionally modified

- `Source/NullTide/Content/InteractionSystem/Inventory/Tests/InventoryComponentTestTypes.h`: added a transient reflected legacy-array test fixture and its `ActorComponent` include.
- `Content/LevelPrototyping/InventorySystem/BP_PickUpItem.uasset`: changed its overlap transaction and destruction flow using Unreal MCP.

### Additional compiler resaves found on resume

- `Content/LevelPrototyping/InventorySystem/UI/Widgets/W_Inventory.uasset`
- `Content/TopDown/Blueprints/BP_TopDownCharacter.uasset`

The previous editor log records both being saved from the exit dialog at `2026-09-15 18:41:43 UTC` (`2026-09-16 01:41:43` Bangkok). No new Blueprint save or gameplay edit was performed during the resumed work. Existing staged PNG icon additions under `Content/Items/Icons` were left untouched.

## Compatibility API

`ULegacyInventoryCompatibilityLibrary` is a native Blueprint function library in `/Script/NullTide`.

```cpp
UFUNCTION(BlueprintCallable, Category = "Inventory|Compatibility")
static FInventoryOperationResult TryAddDefinitionToLegacyInventory(
    UActorComponent* LegacyInventory,
    TSubclassOf<UItemDefinition> DefinitionClass);

UFUNCTION(BlueprintPure, Category = "Inventory|Compatibility")
static TArray<TSubclassOf<UItemDefinition>> GetLegacyItemsSnapshot(
    UActorComponent* LegacyInventory,
    EInventoryOperationResult& Result);
```

The transaction bridge discovers the reflected `InventoryItems` property on the supplied component. It requires an array of class references whose allowed class derives from `UItemDefinition`. It validates the receiver, definition class, definition CDO, and fragment entries before appending anything.

| Result | Meaning |
| --- | --- |
| `Success` | Exactly one definition class was appended to the legacy array. |
| `InvalidDefinition` | The definition is null, abstract, deprecated, or superseded. |
| `InvalidDefinitionData` | The definition CDO or a fragment entry is invalid. |
| `NotInitialized` | The receiver is unusable or does not have the required legacy array schema. |
| `Busy` | A mutation through this bridge is already active for the same component. |

All validation precedes the append, so rejected operations do not mutate the array. The bridge preserves append order and repeated-definition multiplicity. The query returns a copied class-reference array; editing it cannot change authoritative membership.

For a legacy success, `FInventoryOperationResult.Item` remains null and `ItemId` remains invalid. M2 deliberately creates no `UItemInstance`, identity map, mirror array, native revision, or new change-event source.

The existing Blueprint `AddItem(ItemDefinitions)` signature and graph remain unchanged: they append the supplied class to `InventoryItems`. Existing Blueprint-exposed APIs were not removed, renamed, or reparented.

## BP_PickUpItem graph change

The old destructive path called legacy `AddItem` and then unconditionally destroyed the pickup after finding an inventory component.

The persisted MCP graph now performs this sequence:

1. Reject overlap handling if the pickup is already being destroyed.
2. Find the other actor's existing Blueprint `InventoryManagerComponent`.
3. Continue only if that receiver is valid.
4. Call `TryAddDefinitionToLegacyInventory(receiver, ItemDefinition)` once.
5. Call `FInventoryOperationResult::IsSuccess()`.
6. Destroy the pickup only on success.

The invalid-receiver and unsuccessful-result paths have no destruction node. The graph has one transaction call and one destruction call. Synchronous destruction plus the `IsActorBeingDestroyed` guard prevents later overlap callbacks from awarding the same actor again.

All graph writes, compilation, and the intentional pickup save used Unreal MCP. Production `.uasset` bytes were never edited directly. Runtime definition and transform changes used for PIE testing were limited to PIE instances and were discarded when each session ended.

## Build and native tests

| Check | Result |
| --- | --- |
| Target | `NullTideEditor` |
| Platform / configuration | `Win64 Development` |
| Installed engine | Unreal Engine `5.8.1` |
| Final native build | **Succeeded**; module linked successfully. |
| Command-line M2 automation | **4 passed, 0 failed**. |
| Live-editor M2 automation through MCP | **4 passed, 0 failed, 0 skipped**; no test warnings or errors. |

Tests under `NullTide.Inventory.M2`:

- `LegacyValidAdd`: one correct class appended; no native item instance or GUID created.
- `LegacyInvalidDefinitionDoesNotMutate`: null definition rejected without an append.
- `LegacyInvalidReceiverDoesNotMutate`: a concrete component without the legacy schema rejected.
- `LegacySnapshotIsIsolated`: clearing the returned snapshot leaves authoritative membership intact.

Implementation validation fixed a missing `ActorComponent` include in the reflected test fixture. An initial test attempt also instantiated abstract `UActorComponent`; the fixture was corrected to use concrete `UInventoryComponent` as a receiver without the legacy array, then rebuilt and rerun successfully. No unrelated gameplay error was repaired.

Build warnings were the existing newer-than-preferred MSVC toolchain (`14.51.36257`, preferred `14.50.35717`) and backward-compatible `Unreal5_6` include order. No M2 compilation errors remain.

Evidence: final build in `%LOCALAPPDATA%/UnrealBuildTool/Log.txt`; live test and Blueprint compile records in `Saved/Logs/NullTide-backup-2026.09.15-18.41.46.log`.

## Blueprint and PIE validation

All eight existing inventory-related Blueprints compiled through MCP with `warnings_as_errors=true` and no compilation warning or error:

| Asset | Validation |
| --- | --- |
| `InventoryManagerComponent` | Existing `ActorComponent` parent and `AddItem` API preserved. |
| `BP_PickUpItem` | New result-aware overlap graph compiled and saved. |
| `Item_Wood` | Parent `/Script/NullTide.ItemDefinition`; original serialized asset unchanged. |
| `Item_Sword` | Parent `/Script/NullTide.ItemDefinition`; original serialized asset unchanged. |
| `W_Inventory` | Still reads legacy `InventoryItems` on Construct and creates legacy slots. |
| `W_InventorySlot` | Existing class-reference input retained. |
| `BP_TopDownCharacter` | Existing Blueprint inventory component retained; no native authority added. |
| `BP_TopDownController` | Existing inventory-open input path retained. |

Controlled PIE checks ran in `/Game/TopDown/Lvl_TopDown`:

| Scenario | Observed result |
| --- | --- |
| Wood pickup | Array changed from `[]` to `[Item_Wood]`; Wood pickup disappeared; later observation still contained one Wood entry. |
| Sword pickup | Array changed from `[Item_Wood]` to `[Item_Wood, Item_Sword]`; Sword pickup disappeared; later observation remained at two entries. |
| Invalid/null definition | A PIE pickup's definition was set to null before overlap. Array remained empty; both placed pickups remained in the world. |
| Receiver without inventory | Valid Wood pickup was moved into an existing `BP_Sword` actor in PIE. That actor has only its scene root, sword mesh, and sphere components. Inventory remained empty and the pickup remained alive. |
| Failed transaction | Null-definition rejection left the pickup alive and produced no inventory append. |
| UI parity | After acquiring Wood and Sword in a fresh PIE session, opening `W_Inventory` rendered two item slots. MCP inspection confirmed the widget still constructs slots from the legacy class-reference array. |

The tests observed exact array membership and pickup actor presence through MCP. The graph's single guarded destruction path was also inspected; a separate destruction-delegate counter was not instrumented. No capacity or other gameplay failure policy was introduced in M2.

A preliminary attempt to spawn a test actor during PIE was safely rejected by MCP. A preliminary attempt to remove the inherited inventory component from a PIE character was also rejected; the no-inventory check instead used the existing `BP_Sword` actor. Neither attempt changed a production asset or the map.

## Asset scope verification

A SHA-256 baseline captured all **269** project `.uasset` files before the intentional pickup edit. Immediately after the gameplay tests, only `BP_PickUpItem.uasset` differed from that baseline. After the interruption and editor exit save, the resumed comparison found three differences: pickup, widget, and character. No assets were added or deleted.

Read-only `DiffAssets` exports compared the two additional saves with their repository versions:

- `W_Inventory`: exports match after temporary package-path and GUID normalization. Its widget tree, class references, defaults, and graph behavior are unchanged.
- `BP_TopDownCharacter`: exports match after the same normalization plus swapping compiler-generated `K2Node_CallFunction_6` / `_7` names inside `ExecuteUbergraph_BP_TopDownCharacter`. Its authored graphs, component templates, defaults, and dependencies are unchanged.

The first widget diagnostic returned a nonzero commandlet status because the running editor already owned MCP port 8000; both text exports completed. The character diagnostic disabled MCP and completed with zero warnings or errors. These exports are generated comparison artifacts under `Saved/Diff`, not production asset edits.

Thus **no unrelated gameplay asset change was found**, but **byte-for-byte unchanged status is not satisfied for the two compiler resaves**. They remain separate unstaged changes in the resumed repository. No direct-byte restoration was attempted.

On resume, MCP is connected to the running NullTide editor, the world is `/Game/TopDown/Lvl_TopDown`, and PIE is stopped. Item parents and the saved pickup/legacy-add graphs were rechecked. Loading/inspection marked manager and pickup packages dirty in memory; neither was saved during resume. The other six checked Blueprint packages were clean in memory.

## Remaining risks and boundary limits

- The temporary reflection bridge depends on the exact `InventoryItems` property name and class-array schema. Schema changes fail explicitly with `NotInitialized`.
- Old `AddItem` calls and direct legacy array writes still bypass the new validation boundary. They remain available for compatibility; the pickup now uses the result-aware path.
- The bridge is intended for game-thread Blueprint gameplay use. It introduces no asynchronous, replicated, save/load, transfer, equipment, stacking, or quantity behavior.
- Legacy success has no instance pointer or GUID. Callers must branch on `Result` / `IsSuccess`, not on `Item` validity.
- The existing UI constructs a snapshot when opened; live UI event migration remains a later milestone.
- The two compiler resaves are documented repository churn and must not be mistaken for a UI or character migration.

Stop point: **M2 only. No M3 reparenting or inventory authority cutover was performed.**
