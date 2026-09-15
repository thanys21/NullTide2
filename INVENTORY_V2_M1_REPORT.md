# Inventory V2 M1 Implementation Report

Date: 2026-09-16  
Target: `NullTideEditor`  
Platform/configuration: Win64 Development  
Scope: Inventory V2 milestone M1 only

## Outcome

M1A through M1E are implemented and validated. The native foundation now provides a canonical fragment base, per-unit item identity, explicit operation results, an inventory authority component, and focused automation coverage. The production Blueprint inventory remains on its existing implementation: no Blueprint was reparented, migrated, saved, renamed, moved or deleted, and M2 work has not started.

## Files added

- `Source/NullTide/Content/InteractionSystem/Fragments/ItemFragment.h`
- `Source/NullTide/Content/InteractionSystem/Fragments/ItemFragment.cpp`
- `Source/NullTide/Content/InteractionSystem/Items/ItemInstance.h`
- `Source/NullTide/Content/InteractionSystem/Items/ItemInstance.cpp`
- `Source/NullTide/Content/InteractionSystem/Inventory/InventoryTypes.h`
- `Source/NullTide/Content/InteractionSystem/Inventory/InventoryComponent.h`
- `Source/NullTide/Content/InteractionSystem/Inventory/InventoryComponent.cpp`
- `Source/NullTide/Content/InteractionSystem/Inventory/Tests/InventoryComponentTestTypes.h`
- `Source/NullTide/Content/InteractionSystem/Inventory/Tests/InventoryComponentTests.cpp`
- `INVENTORY_V2_M1_REPORT.md`

## Files modified

- `Source/NullTide/Content/InteractionSystem/Fragments/InventoryItemFragment.h`
  - `UInventoryItemFragment` now derives from `UItemFragment`.
  - Its reflected class name, module path and authoring flags remain available for serialized compatibility.

No production `.uasset`, config, module rules, target rules or existing item-definition files were changed by M1.

## Implementation by step

### M1A — Canonical item fragment

`UItemFragment` is an abstract, const, Blueprintable/BlueprintType, default-to-instanced, edit-inline `UObject` base for static capability data. `UInventoryItemFragment` remains the reflected compatibility class at `/Script/NullTide.InventoryItemFragment` and now inherits from it. `UItemDefinition::Fragments` retains its existing name and `TArray<TObjectPtr<UInventoryItemFragment>>` type, so no serialized Blueprint data conversion occurs in M1.

### M1B — Item instance

`UItemInstance` contains only:

- private reflected `FGuid InstanceId`
- private reflected `TSubclassOf<UItemDefinition> DefinitionClass`

Initialization is private, one-shot and accessible only to `UInventoryComponent`. There is no Blueprint or public C++ setter. Each property has a Blueprint-pure getter.

### M1C — Operation types

`EInventoryOperationResult` contains exactly:

- `Success`
- `InvalidDefinition`
- `InvalidDefinitionData`
- `InvalidItemId`
- `NotInitialized`
- `Busy`

`FInventoryOperationResult` contains `Result`, `Item` and `ItemId`, plus a C++ `IsSuccess()` member. `UInventoryOperationResultLibrary::IsSuccess` exposes the same check as a Blueprint-pure script method. The enum uses Python `ScriptName = InventoryOperationResultCode` to avoid a Python name collision with the struct; C++ and Blueprint names are unchanged.

### M1D — Inventory component

`UInventoryComponent` is a Blueprint-spawnable `UActorComponent`. It has no UI or production Blueprint dependency. Its authoritative membership is:

```cpp
UPROPERTY()
TArray<TObjectPtr<UItemInstance>> Items;
```

`Items` is private and has no Blueprint property exposure. Successful adds create one `UItemInstance` per acquired unit with `UInventoryComponent` as its `Outer`, assign a fresh nonzero GUID, and append it to preserve stable insertion order. Successful removes preserve the relative order of remaining entries.

Validation completes before membership changes. Failed operations leave `Items` and `Revision` unchanged. The mutation guard remains active through `OnInventoryChanged.Broadcast`, so mutation attempted by an event callback returns `Busy`. Notification occurs once and only after membership and revision are committed.

Current definition-data validation rejects an invalid CDO or null legacy fragment entry. An empty fragment array is valid.

### M1E — Native tests

Nine editor automation tests were added under `NullTide.Inventory.M1`:

1. `ValidAdd`
2. `RepeatedDefinitionHasUniqueIds`
3. `InvalidDefinition`
4. `RemoveValidItem`
5. `RemoveInvalidId`
6. `SnapshotMembershipIsIsolated`
7. `RevisionOnlyChangesOnSuccess`
8. `EventFiresOnceAfterSuccess`
9. `ReentrantMutationReturnsBusy`

The test helpers are transient and non-Blueprintable. The tests also verify component ownership of instances and stable insertion order.

## Public Blueprint API

### `UItemInstance`

- `GetInstanceId() -> FGuid` — BlueprintPure
- `GetDefinitionClass() -> TSubclassOf<UItemDefinition>` — BlueprintPure

### `FInventoryOperationResult`

- `Result` — Blueprint read-only
- `Item` — Blueprint read-only
- `ItemId` — Blueprint read-only
- `Is Success` — BlueprintPure script method

### `UInventoryComponent`

- `TryAddDefinition(DefinitionClass) -> FInventoryOperationResult`
- `TryRemoveItem(ItemId) -> FInventoryOperationResult`
- `GetItemsSnapshot() -> TArray<UItemInstance*>` — BlueprintPure
- `FindItemById(ItemId) -> UItemInstance*` — BlueprintPure
- `ContainsItem(ItemId) -> bool` — BlueprintPure
- `GetItemCount() -> int32` — BlueprintPure
- `GetRevision() -> int32` — BlueprintPure
- `OnInventoryChanged(NewRevision)` — Blueprint-assignable multicast event
- `Revision` — Blueprint read-only reflected state

All previously exposed `UItemDefinition` and `UInventoryItemFragment` APIs remain present. No legacy Blueprint API was renamed or removed.

## Build result

| Gate | Result |
|---|---|
| M1A fragment base and compatibility inheritance | Succeeded |
| M1B item instance | Succeeded |
| M1C inventory operation types | Succeeded |
| M1D inventory component | Succeeded |
| M1E tests | Succeeded |
| Final build after warning cleanup | Succeeded |

UnrealBuildTool initially declined two attempts because Live Coding was active; compilation did not start on those attempts. The editor was closed cleanly through Unreal MCP and each gate was rerun successfully. No C++ or UHT compile error remained.

Build warnings:

- Visual Studio compiler 14.51.36257 is newer than Unreal's preferred 14.50.35717 toolchain.
- The targets retain the existing backward-compatible `EngineIncludeOrderVersion.Unreal5_6` setting.

These warnings predate M1 and were left unchanged. An M1-specific Python enum/struct naming warning was fixed and did not recur.

## Test result

The final command-line automation run found and passed 9 of 9 tests. The same tests were then discovered and run through Unreal MCP's `AutomationTestToolset`: 9 passed, 0 failed, 0 skipped, with zero per-test warnings or errors.

The unattended editor log also contained unrelated environment warnings for audio sample-rate conversion, navigation-mesh serialized sizing, an old editor-layout key, MCP EULA notice and unavailable non-Win64 platform SDKs. Win64 SDK validation succeeded.

## Unreal MCP and Blueprint validation

The rebuilt editor launched successfully and MCP connected to the running NullTide project. MCP reported current level `/Game/TopDown/Lvl_TopDown` and resolved these new/compatibility native classes:

- `/Script/NullTide.ItemFragment`
- `/Script/NullTide.InventoryItemFragment`
- `/Script/NullTide.ItemInstance`
- `/Script/NullTide.InventoryComponent`

MCP reflection confirmed that `UItemInstance` exposes only `instanceId` and `definitionClass` as reflected instance state. `UInventoryComponent` reflection exposes `revision` and `onInventoryChanged`; private `Items` is not exposed in the scripting property schema.

All original eight inventory-related Blueprints loaded and compiled with `warnings_as_errors = true`:

| Asset | Parent after M1 | Compile |
|---|---|---|
| `InventoryManagerComponent` | `/Script/Engine.ActorComponent` | Success |
| `BP_PickUpItem` | `/Script/Engine.Actor` | Success |
| `Item_Wood` | `/Script/NullTide.ItemDefinition` | Success |
| `Item_Sword` | `/Script/NullTide.ItemDefinition` | Success |
| `W_Inventory` | `/Script/UMG.UserWidget` | Success |
| `W_InventorySlot` | `/Script/UMG.UserWidget` | Success |
| `BP_TopDownCharacter` | `/Script/Engine.Character` | Success |
| `BP_TopDownController` | `/Script/Engine.PlayerController` | Success |

After compilation, MCP reported all eight assets clean. SHA-256 comparisons before and after validation were identical for every `.uasset`, confirming that validation did not modify them on disk.

Item defaults remain:

| Asset | Name | Description | Icon | Fragments |
|---|---|---|---|---|
| `Item_Wood` | `Wood` | `Wood nè` | `/Engine/EngineResources/AICON-Green` | Empty |
| `Item_Sword` | `Sword` | `Sword nè` | `/Engine/EngineResources/AICON-Red` | Empty |

Both retain `/Script/NullTide.ItemDefinition` as their native parent.

## Remaining known issues and deferred work

- Production inventory still uses the existing Blueprint `InventoryManagerComponent`, class-reference array, pickup flow and widgets. This is intentional until later milestones.
- M1 definition-data validation covers the current legacy fragment array and null fragment entries. Canonical fragment storage, duplicate/capability policy and richer per-fragment validation remain M6 work.
- `NotInitialized` currently covers invalid component object lifetime states. Legacy seed/import initialization is deferred to the compatibility cutover milestones.
- The reflected transient test helpers compile in the runtime module because the project has no separate test module. They are non-Blueprintable and never instantiated outside automation; a dedicated test module can absorb them during later source-layout work.
- Stacking, quantity, equipment, save/load, replication, transfers and world-drop spawning remain unimplemented as required.
- Existing source folders remain in place. Layout changes remain M7 work.
- The three production `.uasset` changes already present in Git before M1 remain present and were not changed by this implementation or validation.

M2 has not started.
