# Inventory V2 M7 — source/layout cleanup and retirement review

M7 is complete. Baseline: `09b4052` (M6 complete and committed). Changes are uncommitted. M8 has not started.

33 existing files were moved into the module Public/Private layout. All implementation and test behavior is preserved. Required compatibility APIs remain available; two unused Blueprint authoring/query surfaces received deprecation metadata. Both Development targets, 43 permanent tests, eight warnings-as-errors Blueprint compiles, fresh reflection, rendered PIE and the Windows prototype cook passed. Final Content hashes match the baseline: 280 assets/maps, zero changed, added or removed.

## Audit and retirement decisions

Before editing source, the retirement matrix was recorded in M7_PROGRESS.md. The audit inspected current C++ declarations/callers, all 20 Blueprint assets and 165 graphs with detailed legacy-node pins (zero inspection errors), native reflection and both authored item definitions. It distinguished disconnected serialized legacy nodes from connected production calls. Source scanning included hidden/ignored files under Source.
| Surface | Classification | Evidence / M7 action |
| --- | --- | --- |
| InventoryManagerComponent.InventoryItems | KEEP | Serialized Blueprint variable, native seed/projection reflection adapter and compatibility/native tests depend on exact name/type. No current UI reader. Retain asset/data; no BP metadata edit. New code uses GetItemsSnapshot/transactions. |
| Native LegacyInventoryStorage adapter | KEEP | Native seed import reads once; native projection writes after commit; rollback boundary reads/writes legacy mode. Both InventoryComponent and compatibility library use it. Move header/cpp to Private/Inventory; retain all bodies. |
| InventoryManagerComponent.AddItem(ItemDefinitions) | MIGRATE_LATER | Serialized AddItem call remains in old pickup subgraph; M4 test invokes reflected function. Connected current manager function routes wrapper/native transaction; cannot delete/rename or warn existing nodes. Retain unchanged. |
| Old direct InventoryItems getter/Array_Add nodes in manager AddItem graph | MIGRATE_LATER | Detailed pins show Array_Add execute pin unconnected; function entry now executes wrapper. Nodes still serialized. Preserve graph/asset in M7; eventual Unreal-aware orphan-node retirement requires separate approval. |
| Old pickup AddItem/destruction subgraph | MIGRATE_LATER | Current overlap entry's DSL uses result-aware wrapper/success branch; old serialized AddItem chain still exists. Keep asset unchanged; not proof safe to remove public API. |
| ULegacyInventoryCompatibilityLibrary.TryAddDefinitionToLegacyInventory | KEEP | Connected manager AddItem and production pickup call it. Handles native dispatch and rollback boundary; permanent M1/M2/M4/M6 tests use it. No DeprecatedFunction flag on this API. Add documentation/tooltip pointing new code to native TryAddDefinition. |
| ULegacyInventoryCompatibilityLibrary.GetLegacyItemsSnapshot | DEPRECATE | No call nodes in all 165 Blueprint graphs; still used by native compatibility/projection tests and rollback. Preserve signature/implementation; add DeprecatedFunction/DeprecationMessage for new BP usage only, no UE_DEPRECATED C++ warning. |
| UItemDefinition.Fragments | DEPRECATE | Empty on both current definitions; no Blueprint getter/setter/class-default pin references found. Required serialized property/type, resolver fallback and native compatibility tests. Preserve name/type/flags/storage; add DeprecatedProperty/DeprecationMessage metadata directing new authoring to ItemFragments. |
| UInventoryItemFragment reflected compatibility class | KEEP | Fragments property's reflected type, native resolver compatibility-root exception and legacy test fixtures need it. Preserve /Script/NullTide.InventoryItemFragment and all class flags; explanatory tooltip/comment only. Do NOT set CLASS_Deprecated, which would cause resolver to reject fallback templates. |
| bLegacyInventoryMode / bImportLegacyInventoryOnBeginPlay | KEEP | Production manager seed import, one-time initialization, rollback and permanent tests require defaults/state. Retain unchanged. |
| Initialization/cutover/query compatibility APIs and enum values | KEEP | BeginPlay/import/transaction guards, manager defaults and tests depend on them; preserve all APIs/authority semantics. |
| Legacy W_InventorySlot.ItemDefinition input and precedence branch | KEEP | Serialized rollback input; M5 stale/removed-instance and fallback behavior retained. No asset or warning metadata edit. |
| Permanent M1–M6 tests and reflected fixtures | KEEP | 43 permanent cases and UHT fixtures; move to Private/Tests with includes only. Preserve exact names/behavior. |
| Abandoned plain InventoryItemFragment, duplicate headers/generated Source files, temporary M5 harness | RETIRE (already absent) | Full hidden/no-ignore Source scan: 38 files, no duplicate basenames, no abandoned plain class/generated files/temp UI harness. No remaining file meets removal gate. Archived Saved validation harness/history retained. |

No native API/file met the removal gate. The abandoned plain InventoryItemFragment, duplicate headers, generated files in Source and temporary M5 runtime harness were already absent. Empty old source directories were removed only after their files were moved. Saved historical validation artifacts were retained.

## Source layout and exact file moves

All paths below are project-relative. The module root files and both Target.cs files are unchanged. Private/Inventory/LegacyInventoryStorage.h is an intentional addition to the requested example layout: two implementations require this private adapter.

| Previous file | Current file |
| --- | --- |
| `Source/NullTide/Content/InteractionSystem/Fragments/InventoryItemFragment.cpp` | `Source/NullTide/Private/Compatibility/InventoryItemFragment.cpp` |
| `Source/NullTide/Content/InteractionSystem/Inventory/LegacyInventoryCompatibilityLibrary.cpp` | `Source/NullTide/Private/Compatibility/LegacyInventoryCompatibilityLibrary.cpp` |
| `Source/NullTide/Content/InteractionSystem/Inventory/InventoryComponent.cpp` | `Source/NullTide/Private/Inventory/InventoryComponent.cpp` |
| `Source/NullTide/Content/InteractionSystem/Inventory/LegacyInventoryStorage.cpp` | `Source/NullTide/Private/Inventory/LegacyInventoryStorage.cpp` |
| `Source/NullTide/Content/InteractionSystem/Inventory/LegacyInventoryStorage.h` | `Source/NullTide/Private/Inventory/LegacyInventoryStorage.h` |
| `Source/NullTide/Content/InteractionSystem/Fragments/ItemFragment.cpp` | `Source/NullTide/Private/Items/Fragments/ItemFragment.cpp` |
| `Source/NullTide/Content/InteractionSystem/Fragments/ItemFragmentCapabilities.cpp` | `Source/NullTide/Private/Items/Fragments/ItemFragmentCapabilities.cpp` |
| `Source/NullTide/Content/InteractionSystem/Items/ItemDefinition.cpp` | `Source/NullTide/Private/Items/ItemDefinition.cpp` |
| `Source/NullTide/Content/InteractionSystem/Items/ItemInstance.cpp` | `Source/NullTide/Private/Items/ItemInstance.cpp` |
| `Source/NullTide/Content/InteractionSystem/Inventory/Tests/InventoryComponentTests.cpp` | `Source/NullTide/Private/Tests/InventoryComponentTests.cpp` |
| `Source/NullTide/Content/InteractionSystem/Inventory/Tests/InventoryComponentTestTypes.h` | `Source/NullTide/Private/Tests/InventoryComponentTestTypes.h` |
| `Source/NullTide/Content/InteractionSystem/Inventory/Tests/InventoryCutoverTests.cpp` | `Source/NullTide/Private/Tests/InventoryCutoverTests.cpp` |
| `Source/NullTide/Content/InteractionSystem/Inventory/Tests/ItemFragmentTests.cpp` | `Source/NullTide/Private/Tests/ItemFragmentTests.cpp` |
| `Source/NullTide/Content/InteractionSystem/Inventory/Tests/ItemFragmentTestTypes.h` | `Source/NullTide/Private/Tests/ItemFragmentTestTypes.h` |
| `Source/NullTide/Content/InteractionSystem/Inventory/Tests/LegacyInventoryCompatibilityTests.cpp` | `Source/NullTide/Private/Tests/LegacyInventoryCompatibilityTests.cpp` |
| `Source/NullTide/Content/InteractionSystem/Fragments/InventoryItemFragment.h` | `Source/NullTide/Public/Compatibility/InventoryItemFragment.h` |
| `Source/NullTide/Content/InteractionSystem/Inventory/LegacyInventoryCompatibilityLibrary.h` | `Source/NullTide/Public/Compatibility/LegacyInventoryCompatibilityLibrary.h` |
| `Source/NullTide/Content/InteractionSystem/Inventory/InventoryComponent.h` | `Source/NullTide/Public/Inventory/InventoryComponent.h` |
| `Source/NullTide/Content/InteractionSystem/Inventory/InventoryTypes.h` | `Source/NullTide/Public/Inventory/InventoryTypes.h` |
| `Source/NullTide/Content/InteractionSystem/Fragments/ItemFragment.h` | `Source/NullTide/Public/Items/Fragments/ItemFragment.h` |
| `Source/NullTide/Content/InteractionSystem/Fragments/ItemFragment_Ammo.h` | `Source/NullTide/Public/Items/Fragments/ItemFragment_Ammo.h` |
| `Source/NullTide/Content/InteractionSystem/Fragments/ItemFragment_Consumable.h` | `Source/NullTide/Public/Items/Fragments/ItemFragment_Consumable.h` |
| `Source/NullTide/Content/InteractionSystem/Fragments/ItemFragment_Drink.h` | `Source/NullTide/Public/Items/Fragments/ItemFragment_Drink.h` |
| `Source/NullTide/Content/InteractionSystem/Fragments/ItemFragment_Durability.h` | `Source/NullTide/Public/Items/Fragments/ItemFragment_Durability.h` |
| `Source/NullTide/Content/InteractionSystem/Fragments/ItemFragment_Equippable.h` | `Source/NullTide/Public/Items/Fragments/ItemFragment_Equippable.h` |
| `Source/NullTide/Content/InteractionSystem/Fragments/ItemFragment_Food.h` | `Source/NullTide/Public/Items/Fragments/ItemFragment_Food.h` |
| `Source/NullTide/Content/InteractionSystem/Fragments/ItemFragment_Healing.h` | `Source/NullTide/Public/Items/Fragments/ItemFragment_Healing.h` |
| `Source/NullTide/Content/InteractionSystem/Fragments/ItemFragment_Resource.h` | `Source/NullTide/Public/Items/Fragments/ItemFragment_Resource.h` |
| `Source/NullTide/Content/InteractionSystem/Fragments/ItemFragment_Tool.h` | `Source/NullTide/Public/Items/Fragments/ItemFragment_Tool.h` |
| `Source/NullTide/Content/InteractionSystem/Fragments/ItemFragment_Weapon.h` | `Source/NullTide/Public/Items/Fragments/ItemFragment_Weapon.h` |
| `Source/NullTide/Content/InteractionSystem/Fragments/ItemFragmentTypes.h` | `Source/NullTide/Public/Items/Fragments/ItemFragmentTypes.h` |
| `Source/NullTide/Content/InteractionSystem/Items/ItemDefinition.h` | `Source/NullTide/Public/Items/ItemDefinition.h` |
| `Source/NullTide/Content/InteractionSystem/Items/ItemInstance.h` | `Source/NullTide/Public/Items/ItemInstance.h` |

There are still 38 Source files and no duplicate basenames. Public headers now use module-relative includes (`Items/`, `Items/Fragments/`, `Inventory/`, `Compatibility/`); private implementations use those same public paths, and local test fixtures retain same-directory includes. Generated-header basenames/order are unchanged. The 12 cpp files and two fixture headers match their previous bodies after removing quoted include lines. Of all 33 files, only the following three headers have additional metadata/comment changes:

- Public/Items/ItemDefinition.h: Fragments retains its name, TArray<TObjectPtr<UInventoryItemFragment>> type, category and property flags. Added DeprecatedProperty/DeprecationMessage directing new authoring to ItemFragments/UItemFragment, plus a legacy comment. Generated Editor and game UHT flags were checked: CPF_Deprecated is absent, so storage/serialization is preserved.
- Public/Compatibility/LegacyInventoryCompatibilityLibrary.h: GetLegacyItemsSnapshot retains its signature/body and gets DeprecatedFunction/DeprecationMessage directing new code to GetItemsSnapshot. No Blueprint caller was found; native tests remain valid because no C++ UE_DEPRECATED annotation was added. The active TryAddDefinitionToLegacyInventory is retained without deprecation and gets a documentation/tooltip pointer to TryAddDefinition.
- Public/Compatibility/InventoryItemFragment.h: added a compatibility comment/tooltip. The class is not flagged Deprecated: M6 validation rejects deprecated template classes, so such a flag would break legacy fallback.

All other moved files differ only in include paths, or are byte-equivalent in line content. No reflected class/property/function was renamed. No CoreRedirect was added/removed. No module dependency was added: Core, CoreUObject, Engine and InputCore remain the runtime dependencies. No gameplay, authority, initialization, transaction, UI event/snapshot or capability implementation changed.

## Blueprint/content results

No Blueprint graph, default or content asset was intentionally edited or saved through MCP. After the source rebuild, a fresh UE 5.8.1 editor connected through MCP on port 8001. The following eight original Blueprints compiled with warnings_as_errors=true both before and after restoring compiler resaves:

| Blueprint | Native parent after validation | Result |
| --- | --- | --- |
| InventoryManagerComponent | /Script/NullTide.InventoryComponent | Passed |
| BP_PickUpItem | /Script/Engine.Actor | Passed |
| Item_Wood | /Script/NullTide.ItemDefinition | Passed |
| Item_Sword | /Script/NullTide.ItemDefinition | Passed |
| W_Inventory | /Script/UMG.UserWidget | Passed |
| W_InventorySlot | /Script/UMG.UserWidget | Passed |
| BP_TopDownCharacter | /Script/Engine.Character | Passed |
| BP_TopDownController | /Script/Engine.PlayerController | Passed |

The original /Script/NullTide.ItemDefinition, ItemInstance, ItemFragment, InventoryItemFragment and InventoryComponent reflected paths all resolve. Reflected property names/types are unchanged; the Fragments schema description now explains legacy authoring. Canonical and legacy properties resolve; Blueprint compilation found no broken native parent, pin or type. Manager defaults and serialized legacy fields remain intact because the final asset files are identical to M6.

### Compiler-only resaves and final scope

When the original editor closed, 12 Blueprint files were resaved together at 19:50:22. The changes preceded our fresh validation session. The user explicitly confirmed that they were compiler-only resaves and authorized backup/restoration. Every resave was backed up and hash-verified in Saved/Validation/InventoryV2M7/ClosureResavesBackup, then restored from baseline HEAD through source control with the editor closed. No uasset bytes were hand-edited and no authored changes were discarded.

The 12 reviewed files were ABP_Unarmed; prototype BP_Door, BP_JumpPad, BP_Sword, BP_WobbleTarget; production BP_PickUpItem, W_Inventory, W_InventorySlot, PlayerInteractionComponent, WB_interactionWidget, BP_TopDownCharacter and BP_TopDownController. Full paths/hashes/timestamps are in content-resaves-review.json. This restoration preserved the existing prototype assets; it performed no M8 cleanup.

After restoration, a second fresh editor reran all 43 tests, all eight compiles and rendered PIE. A Windows cook also reran against the restored files. The validation editor then closed normally without saving. Final comparison: 280 assets/maps before and after, zero modified, zero added, zero removed. Item_Wood and Item_Sword never changed on disk.

## Builds and native tests

Associated engine: installed UE 5.8.1 at D:/game/Unreal Engine/UE_5.8. Build configuration: Win64 Development.

| Check | Result | Evidence |
| --- | --- | --- |
| NullTideEditor | Passed, 11.23 s | build-editor.log |
| NullTide game target | Passed, 49.86 s | build-game.log |
| Permanent native tests, fresh restored-assets session | 43 passed; 0 failed/skipped; 0 test errors/warnings | native-tests-final.json |
| Original eight Blueprint compiles | 8 passed; warnings as errors | blueprint-compiles-final.json |
| Fresh native reflection | Paths/property names/types preserved | reflection-before.json, reflection-after.json |

All moved cpp files compiled individually under adaptive non-unity, exercising include hygiene. Permanent test names match the M6 test list. Test organization is now Private/Tests; no test behavior was added/changed.

| Permanent milestone suite | Cases | Result |
| --- | --- | --- |
| M1 | 9 | Passed |
| M2 | 4 | Passed |
| M3 | 2 | Passed |
| M4 | 10 | Passed |
| M6 | 18 | Passed |

M5 lifecycle validation used a temporary harness in its original milestone and has no permanent native suite. That harness was not promoted into runtime source; the M7 live UI PIE regression validates the unchanged production path.

## Rendered PIE regression

Lvl_TopDown was tested in a fresh rendered editor through MCP, including the restored M6 assets. A command-line D3D11 override avoided the previously observed D3D12 timeout; project renderer/configuration files were not changed.

- Away from pickups: one InventoryManagerComponent, NativeReady, legacy mode false, empty projection and Revision 1.
- Press I: the existing owned W_Inventory_C_0 opens with empty inventory.
- Real Wood overlap: one native instance, Wood projection, Revision 2, same open UI updates to one slot.
- Move away, then real Sword overlap: two native instances in [Wood, Sword] order, projection matches, Revision 3, both world pickups removed and the same open UI updates to two slots.
- Current slots reference their native UItemInstance and the existing manager, bUseNativeItem=true, legacy ItemDefinition=None. Controller/pawn/widget ownership references are intact. Each instance Outer is the existing manager component.

Final session item identities:

| Definition | Instance GUID |
| --- | --- |
| Item_Wood.Item_Wood_C | `DE767795-44A2-37F4-6A48-59A9ED790B59` |
| Item_Sword.Item_Sword_C | `76D1427C-4119-6920-9D57-8D88350EFAFC` |

Canonical templates remain definition-owned and unchanged: Wood ResourceType=Wood; Sword WeaponType=Sword, Damage=10, AttackSpeed=1, Range=150, Equippable slot MainHand and MaxDurability=100. Legacy Fragments arrays remain empty. The permanent authored-template/static-state test and post-pickup template inspection both passed; final baseline hashes independently preserve authored defaults.

PIE was stopped normally and Slate observation released. Evidence: pie-initial-final.json, pie-pickup-runtime-final.json, items-after-pie.json and screenshots pie-empty-final.png, pie-wood-open-final.png, pie-wood-sword-open-final.png.

## Win64 cook and generated project files

Game target build passed. The restored-assets cook ran:

```text
UnrealEditor-Cmd.exe NullTide.uproject -run=Cook -TargetPlatform=Windows -Map=/Game/TopDown/Lvl_TopDown -unattended -nop4 -NullRHI -stdout
```

Final cook passed exit 0: 595 cooked packages, 0 remaining, 30.23 s commandlet duration, 0 errors and 1 existing MCP plugin startup warning. The earlier full cook also passed (140.09 s). Packaging settings were unchanged. This validates the prototype Windows map cook; no distributable package/archive was created or launched.

Generated .vscode compile databases still referenced pre-cleanup Content/InteractionSystem source paths. Regenerated VS Code project files with the associated engine via Build.bat -projectfiles -project=<NullTide.uproject> -game -engine -vscode (6.37 s). Both compile databases now reference existing source files in the new layout; no old source-folder or NullTide2 references remain anywhere in .vscode. These IDE files are generated and ignored by source control. Binaries, Intermediate, Saved/Cooked and validation logs are generated output, not source-of-truth edits.

## Warnings, limitations and retained risks

- Existing MSVC 14.51.36257 is newer than Unreal's preferred 14.50.35717; existing Unreal5_6 include-order notice remains. The first game SharedPCH emitted engine-header C4996 deprecations. No new project-source warnings or include/deprecation compile failures resulted from M7.
- ObjectTools cannot describe some existing dynamic delegate properties as JSON schemas (including OnInventoryChanged). This tooling warning does not affect reflected event behavior or Blueprint compilation; PIE confirmed the live update path.
- The cook's single warning is the existing MCP plugin startup notice. There are no code/content cook errors or infrastructure blockers.
- Default D3D12 rendering was not retested; rendered validation used D3D11 as a command-line override.
- Serialized disconnected legacy nodes, rollback flags, projection adapter, AddItem and compatibility types remain deliberately available. Future retirement requires a separate Blueprint-aware migration/reference review. New code should use native transactions/snapshots and canonical ItemFragments.
- Future external C++ consumers must use the new include paths. Reflected /Script paths are unaffected.

## Recovery and evidence

All evidence is under Saved/Validation/InventoryV2M7 (generated/ignored): source/assets baseline, move-manifest.json, source body comparisons, native/Blueprint/reflection results, PIE runtime/screenshots, both cook results, generated IDE log, resave review/backup and scope-final.json.

To resume review, start from this report and the final checkpoint in M7_PROGRESS.md; implementation and validation are complete. No editor remains running. No M7 work is pending and M8 must not be started automatically.

Rollback point is baseline 09b4052. With editor closed, restore the 33 old source paths and remove only their corresponding moved destinations from the recorded manifest as one coherent source rollback, then regenerate IDE files and rebuild both targets. No content rollback is needed: final Content already equals the baseline. Preserve the resave backups until review is complete. Nothing was staged or committed by this task.
