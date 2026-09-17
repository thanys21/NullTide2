# Inventory V2 M5 report

Status: COMPLETE. M5 only; M6 has not started.
Date: 2026-09-18 (Asia/Bangkok).
Project: NullTide, Unreal Engine 5.8.1.
Baseline: `fa7bb2c284609cae8cc5cb2ff12dba751d48897c` (committed M4).

## Result

W_Inventory now observes the owning pawn's native UInventoryComponent through GetItemsSnapshot and OnInventoryChanged. Native Items remains the sole authoritative membership store. The widget rebuilds immediately on opening and committed inventory changes, preserves insertion order, and releases its own subscriptions on teardown/rebinding. Slots consume UItemInstance references and validate actual membership before showing definition metadata.

InventoryItems remains a derived compatibility projection; AddItem and all existing native/legacy compatibility APIs remain intact. No native gameplay source, module dependencies, project configuration, item assets, pickup, character, folders or fragment data changed in the final implementation.

## Files modified

| File | Change |
| --- | --- |
| Content/LevelPrototyping/InventorySystem/UI/Widgets/W_Inventory.uasset | Native snapshot/event observer, membership-aware ordered slot rebuild, ownership and binding lifecycle. |
| Content/LevelPrototyping/InventorySystem/UI/Widgets/W_InventorySlot.uasset | Instance input, static metadata resolution, membership query, stale-reference clearing, retained legacy fallback. |
| Content/TopDown/Blueprints/BP_TopDownController.uasset | Existing CreateWidget OwningPlayer pin connects to controller self; one new self-reference node. |

Files added: INVENTORY_V2_M5_REPORT.md and M5_PROGRESS.md.

All Blueprint edits and intended asset saves used Unreal MCP. No .uasset bytes were manually edited. Only these three assets were intentionally saved; no unrelated asset restore was necessary.

A temporary editor-only C++ PIE validation harness and temporary UnrealEd/UMG dependencies were used to exercise native removal and inspect widget/delegate lifecycle. After validation, the newly created test source was archived under ignored Saved/Validation/InventoryV2M5/InventoryUIValidationHarness.cpp, and its dependency additions were restored. No permanent native UI helper or native test was introduced, matching the request to add native helper tests only when helper behavior is added. Final Source and Build.cs match the M4 baseline.

## Blueprint API and behavior

### W_Inventory

New helpers: SetupInventoryObserver, RebindInventory, RefreshInventory, ReleaseInventoryBinding, ReleaseBindings, HandleInventoryChanged(NewRevision), and HandlePawnChanged(OldPawn, NewPawn).

Transient observer references: InventoryComponent, OwningController and BoundPawn. These identify the observed owner and hold no authoritative item array.

- Construct releases existing bindings, obtains GetOwningPlayer/GetOwningPlayerPawn, finds UInventoryComponent by native class and builds an initial snapshot.
- Refresh clears InventoryContainerWrapBox before rebuilding. It reads only GetItemsSnapshot in native insertion order, checks membership and identity, creates slots with explicit OwningPlayer, calls SetItemInstance before adding them, and leaves the panel empty when authority/component is unavailable.
- OnInventoryChanged refreshes after successful native commits. UI functions call no inventory mutation transaction.
- Teardown/rebinding removes only this widget's inventory and possession handlers, clears children and releases references. It never clears other listeners.
- OnPossessedPawnChanged reacquires the component. Tick compares owner/pawn/component pointers to repair changed bindings; it does not read snapshots or rebuild continuously while those pointers are unchanged.
- Missing owner, pawn or inventory safely clears the panel/binding. Restoring ownership/possession reacquires a current native snapshot.

Inspection of all final widget graphs found zero InventoryItems reads and zero inventory mutation calls. The existing OnKeyDown graph is unchanged.

### W_InventorySlot

Primary Blueprint-callable input: SetItemInstance(NewItem: UItemInstance, NewInventory: UInventoryComponent). Additional helpers: IsItemActive, RefreshDisplay, ClearDisplay and ApplyDefinitionDisplay.

Fields: ItemInstance, InventoryComponent, bUseNativeItem, ResolvedItemName and ResolvedItemDescription. Existing ItemDefinition class-reference/ExposeOnSpawn input remains available and unchanged for compatibility.

Native input takes precedence when the setter selects native mode or a non-null ItemInstance is supplied directly. Refresh makes native selection sticky: a missing/removed native item cannot subsequently revive the legacy fallback. Active membership requires valid item/component references, ContainsItem(InstanceId), and FindItemById(InstanceId) returning the same object. UObject validity alone is insufficient.

Static ItemName, ItemDescription and ItemIcon resolve from the instance's definition class defaults. Name/description fields, existing icon brush and description tooltip use the shared display helper. Refresh first clears old display; destruction releases input references and clears display.

Legacy fallback applies only when no native mode/input has been selected and ItemDefinition is a valid class. It resolves through the same display helper. The production inventory widget sets native instance input and leaves the legacy class input unset.

### Controller preservation

Detailed before/after node comparison shows the only changed existing controller node is its inventory CreateWidget OwningPlayer pin. One self-reference node was added. All other existing controller nodes/pins, five function/construction graphs and the widget's OnKeyDown are unchanged. Existing I open/close, cursor and input-mode behavior is preserved.

## Build and native validation

Final target: NullTideEditor.
Platform/configuration: Win64 Development.
Associated installed engine: UE 5.8.1.
Final build: SUCCEEDED, 13.62 seconds, with original native source/dependencies.

| Suite | Result |
| --- | --- |
| M1 | 9/9 passed |
| M2 | 4/4 passed |
| M3 | 2/2 passed |
| M4 | 10/10 passed |
| Final existing native suite | 25 passed, 0 failed, 0 skipped; no unexpected warnings |
| Temporary M5 PIEObserverLifecycleAndRemoval harness | Passed; complete harness run 26 passed, 0 failed, 0 skipped; no unexpected warnings |

The temporary harness was rerun after correcting two setup assumptions: the automation runner ends pre-existing PIE, so the test owns latent startup/cleanup; UE SetOwningPlayer(nullptr) is a no-op, so a real missing-owner case uses SetPlayerContext(FLocalPlayerContext()). These were test setup fixes, requiring no inventory/Blueprint authority changes.

An intermediate link attempt failed with LNK1104 while Unreal Editor held the DLL open; the successful final builds followed normal editor closure. Existing nonpreferred MSVC 14.51.36257 (preferred 14.50.35717) and backward-compatible Unreal5_6 include-order notices remain unchanged. No unrelated build system fix was made.

## Blueprint validation

All eight compiled through MCP with warnings_as_errors=true after the final native-baseline rebuild. No missing parents, invalid pins or unresolved compile errors were reported.

| Blueprint | Result | Parent |
| --- | --- | --- |
| InventoryManagerComponent | Passed | /Script/NullTide.InventoryComponent |
| BP_PickUpItem | Passed | /Script/Engine.Actor |
| Item_Wood | Passed | /Script/NullTide.ItemDefinition |
| Item_Sword | Passed | /Script/NullTide.ItemDefinition |
| W_Inventory | Passed | /Script/UMG.UserWidget |
| W_InventorySlot | Passed | /Script/UMG.UserWidget |
| BP_TopDownCharacter | Passed | /Script/Engine.Character |
| BP_TopDownController | Passed | /Script/Engine.PlayerController |

Item_Wood retains name Wood, description Wood nè, AICON-Green icon and empty Fragments. Item_Sword retains name Sword, description Sword nè, AICON-Red icon and empty Fragments. Both original asset hashes are unchanged.

## PIE regression

| Scenario | Evidence/result |
| --- | --- |
| Empty Lvl_TopDown start/open | NativeReady, legacy mode false, native/projection empty; zero slots. Empty seed initialization produces Revision 1, matching M4. |
| Wood pickup while UI closed | Real pickup produces one native Wood instance/projection entry, Revision 2. Reopening through I shows one correctly owned Wood slot and its static metadata/icon. |
| Sword pickup with same UI open | Real pickup updates the same widget immediately to two slots in Wood/Sword order, Revision 3, matching projection. Rendered screenshot confirms both icons. |
| Unique identity | Wood 54DE8FB5-4937-FC32-E3EA-568680F448EA; Sword 72E78F44-46D0-0EED-6880-F9929ADBEAD2 in the manual session; both instances have the inventory component as Outer. |
| Ownership | Widget resolves the intended controller/pawn/component; native harness verifies exactly one inventory component and one active inventory widget. |
| Repeated refresh/reconstruct | Three cycles retain exactly two slots and one widget inventory/possession subscription; teardown clears children/references/subscriptions. Membership and Revision remain unchanged by UI lifecycle. |
| Controlled native removal | TryRemoveItem removes Wood, increments Revision once and immediately leaves the Sword slot/projection. Retained valid removed UObject input fails membership and clears name/icon even with a valid legacy fallback. |
| Possession/missing inventory | Unpossess clears bindings/slots; a pawn without inventory stays empty; repossess restores the original native component and remaining slot without changing membership/Revision. |
| Missing owner/recovery | Clearing actual player context safely clears bindings/panel; restoring owner immediately rebuilds the remaining native snapshot. |

Rendered empty/closed/live pickup tests ran in ordinary PIE through MCP/Slate. The full native/lifecycle harness ran in unattended NullRHI PIE after a later D3D12 editor relaunch logged GPU timeouts and stalled MCP responses. NullRHI verifies widget structure, metadata, bindings and transactions; it does not establish rendered removal/possession appearance. The earlier rendered pickup screenshots are preserved. No renderer/driver/project settings were changed to address this infrastructure issue. GPU-free validation used command-line MCP port 8001 only.

## Asset scope and compatibility

SHA-256 baseline/current count: 280 Content .uasset/.umap files. Exactly the three intended UI/controller assets changed; no assets added, removed, renamed or moved. The other 277 files, including manager, pickup, character, Wood and Sword, are byte-identical to the baseline. No unrelated compiler-only disk resaves needed restoration.

Final source/config/uproject differences: none. Legacy InventoryItems, AddItem, compatibility library, M4 atomic import/NativeReady initialization/native transaction rules, Revision and projection behavior remain intact. UI introduces no second active inventory store. Fragment canonicalization and other M6 work have not begun.

## Remaining risks and rollback

- The D3D12 GPU timeout on one validation relaunch remains an environment issue; no GPU fix was attempted. Functional lifecycle/removal validation is GPU-free, supplemented by prior rendered pickup checks.
- Lvl_TopDown's existing missing-RecastNavMesh warning was narrowly marked expected in the temporary harness. Other test warnings were not suppressed.
- This milestone clears/rebuilds slots on changes. Future persistent slot selection/focus would require explicit reconciliation; no new selection behavior is introduced here.
- Existing compiler preference/include-order notices remain.

R5 rollback: close PIE/editor and restore the coherent three UI/controller assets from the M4 baseline. Existing legacy class input, projection and wrappers remain available for that rollback. Use fresh sessions; do not translate runtime item instances back into an authoritative legacy array. No commit was created by this implementation.

## Validation evidence

Ignored local artifacts: Saved/Validation/InventoryV2M5/. They include pre-change hashes/graphs, final detailed graphs/pins and preserved-function comparisons, rendered PIE screenshots/runtime identities/defaults, complete temporary harness source/results, final native-baseline build/25-test results/eight compile results and final asset-scope JSON. M5_PROGRESS.md records checkpoints and the completed state.
