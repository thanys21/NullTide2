# Inventory V2 M6 ¡ª canonical fragments and capability authoring

Status: complete and validated, 2026-09-18. M6 only; M7 not started. Baseline commit: `7f90933` (M5). No commit created by this implementation.

Canonical static capability composition is now available on UItemDefinition. Wood and Sword use owned inline native templates. Inventory authority and M5 UI ownership/event behavior remain unchanged.

## Resolver and validation

Added `ItemFragments : TArray<TObjectPtr<UItemFragment>>` with Instanced, EditDefaultsOnly, BlueprintReadOnly and Items|Fragments category/tool tip. Original `Fragments : TArray<TObjectPtr<UInventoryItemFragment>>`, its metadata, and `/Script/NullTide.InventoryItemFragment` remain intact. No reflected names changed; no redirects added or removed.

| Definition configuration | Resolution |
| --- | --- |
| Canonical populated, legacy empty | Canonical templates in authored order |
| Canonical empty, legacy populated | Legacy compatibility fallback in authored order |
| Both empty | Valid zero-capability definition |
| Both populated | Invalid; clear source-conflict diagnostic, no merging |
| Null/invalid template | Invalid; source/index diagnostic |
| Duplicate capability or ambiguous subclasses | Invalid; both indices and shared capability class diagnostic |
| Negative/nonfinite numeric configuration | Invalid; indexed field diagnostic |

ResolveFragments stages its output and only publishes the full ordered snapshot after validation succeeds. Validation has no side effects. A shared ancestor other than UItemFragment/UInventoryItemFragment constitutes a duplicate capability: this rejects same-class, parent/child and sibling subclasses of one capability. Independent capability classes compose normally. FindFragmentByClass returns the single compatible match; missing/invalid/ambiguous queries return null. A broad UItemFragment query matching several independent capabilities returns null without invalidating the definition itself.

UInventoryComponent add/import data checks and the legacy transaction boundary delegate to definition validation. Their transaction/cutover/Busy/revision/event/projection implementation and public APIs are otherwise unchanged. Invalid add/import fails atomically with InvalidDefinitionData. Runtime UItemInstance still contains only its existing identity and definition class; fragments remain shared static definition configuration.

## Public Blueprint API

Added BlueprintPure, const methods on UItemDefinition:

- `ResolveFragments(OutFragments, OutDiagnostic) -> bool`
- `ValidateFragments(OutDiagnostic) -> bool`
- `GetResolvedFragments() -> TArray<UItemFragment*>`
- `FindFragmentByClass(FragmentClass) -> UItemFragment*` (output type follows class input)
- `HasFragmentByClass(FragmentClass) -> bool`

Array reads return snapshots; Blueprint cannot write array membership. Template fields are EditDefaultsOnly/BlueprintReadOnly and template classes are Const/DefaultToInstanced/EditInlineNew. Native `UItemFragment::ValidateTemplate(FString&) const` supplies optional pure static-data validation; it exposes no runtime mutation or Blueprint event.

| Native capability | Static configuration |
| --- | --- |
| UItemFragment_Resource | ResourceType (FName) |
| UItemFragment_Consumable | Marker only; no use behavior |
| UItemFragment_Food | HungerRestore |
| UItemFragment_Drink | HydrationRestore |
| UItemFragment_Healing | HealAmount |
| UItemFragment_Equippable | EquipmentSlot |
| UItemFragment_Weapon | WeaponType, Damage, AttackSpeed, AttackRange (cm) |
| UItemFragment_Ammo | AmmoType, DamageModifier |
| UItemFragment_Durability | MaxDurability only |
| UItemFragment_Tool | ToolType, Efficiency |

Minimal BlueprintType enums: EEquipmentSlot (None/MainHand/OffHand), EWeaponType (None/Sword), EAmmoType (None/Arrow), EToolType (None/Axe/Pickaxe). Numeric static configuration must be finite and nonnegative. No GameplayTags, new module dependencies, content-specific item classes, mutable durability/current-state fields or capability gameplay behavior were introduced.

## Files added

All native paths below are relative to `Source/NullTide/Content/InteractionSystem/`.

| New file | Purpose |
| --- | --- |
| Fragments/ItemFragmentTypes.h | Minimal capability enums |
| Fragments/ItemFragment_Resource.h | Resource capability |
| Fragments/ItemFragment_Consumable.h | Consumable marker |
| Fragments/ItemFragment_Food.h | Food capability |
| Fragments/ItemFragment_Drink.h | Drink capability |
| Fragments/ItemFragment_Healing.h | Healing capability |
| Fragments/ItemFragment_Equippable.h | Equipment configuration |
| Fragments/ItemFragment_Weapon.h | Weapon configuration |
| Fragments/ItemFragment_Ammo.h | Ammunition configuration |
| Fragments/ItemFragment_Durability.h | Maximum durability configuration |
| Fragments/ItemFragment_Tool.h | Tool configuration |
| Fragments/ItemFragmentCapabilities.cpp | Pure finite/nonnegative field validators |
| Inventory/Tests/ItemFragmentTestTypes.h | Isolated transient native test fixtures |
| Inventory/Tests/ItemFragmentTests.cpp | Eighteen M6 native regression cases |

Documentation added: INVENTORY_V2_M6_REPORT.md and M6_PROGRESS.md.

## Files modified

| Existing native file (same relative prefix) | Change |
| --- | --- |
| Items/ItemDefinition.h | Additive canonical storage and read/validation API |
| Items/ItemDefinition.cpp | One resolver and query/validation implementation |
| Fragments/ItemFragment.h | Pure native ValidateTemplate declaration |
| Fragments/ItemFragment.cpp | Default zero-configuration validator |
| Inventory/InventoryComponent.cpp | Definition validation delegates to resolver |
| Inventory/LegacyInventoryCompatibilityLibrary.cpp | Same validation for compatibility boundary |

Only content files changed:

- `Content/LevelPrototyping/InventorySystem/Items/Item_Wood.uasset`
- `Content/LevelPrototyping/InventorySystem/Items/Item_Sword.uasset`

Both were authored and saved through Unreal MCP ObjectTools/AssetTools only. No .uasset bytes were edited directly. No Blueprint graph changes. No source/content folders, assets, classes or legacy APIs were moved, deleted or renamed.

## Authored item defaults and persisted ownership

| Item | Canonical templates in order | Legacy | Preserved metadata |
| --- | --- | --- | --- |
| Item_Wood | Resource(ResourceType=Wood) | Empty | Wood; Wood n¨¨; AICON-Green |
| Item_Sword | Weapon(Sword, Damage=10, AttackSpeed=1, AttackRange=150cm), Equippable(MainHand), Durability(MaxDurability=100) | Empty | Sword; Sword n¨¨; AICON-Red |

New numbers are static placeholder configuration, with no combat/equipment/durability behavior. Original asset paths and direct parent `/Script/NullTide.ItemDefinition` remain unchanged. Icons retain their original `/Engine/EngineResources/AICON-Green.AICON-Green` and `AICON-Red.AICON-Red` paths.

Fresh-editor reload and AuthoredTemplatesPersistAndRemainStatic verified exactly one Wood/three Sword templates, class/property instancing flags, direct definition CDO ownership, persisted configuration and no duplicate owned serialized templates. Runtime acquisition creates distinct UItemInstances without copying/replacing templates or changing their configuration. Owned paths are:

- `Item_Wood.Default__Item_Wood_C:ItemFragment_Resource_0`
- `Item_Sword.Default__Item_Sword_C:ItemFragment_Weapon_0`
- `Item_Sword.Default__Item_Sword_C:ItemFragment_Equippable_0`
- `Item_Sword.Default__Item_Sword_C:ItemFragment_Durability_0`

These are subobjects inside the existing item packages; no standalone fragment Blueprint assets were required.

## Build and tests

Target: NullTideEditor. Platform/configuration: Win64 Development. Associated installed engine: UE 5.8.1 at D:/game/Unreal Engine/UE_5.8.

Final production-source build succeeded (2.65 seconds incremental). Earlier foundation and full temporary harness builds also succeeded. Fixed one M6 test-only const UObject pointer comparison compile error; updated the new ownership test to EGetObjectsFlags::None rather than a deprecated boolean overload. No gameplay build errors or unrelated fixes.

| Validation | Result |
| --- | --- |
| M1 existing native tests | 9/9 passed |
| M2 existing native tests | 4/4 passed |
| M3 existing native tests | 2/2 passed |
| M4 existing native tests | 10/10 passed |
| M6 permanent native tests | 18/18 passed |
| Final production suite | 43 passed, 0 failed/skipped; no test errors/warnings |
| Archived M5 lifecycle/removal harness plus full suite | 44 passed, 0 failed/skipped |

M6 covers empty/canonical/legacy resolution, dual sources, null canonical/legacy entries, duplicate capability, parent-child and sibling ambiguity, correct/missing/broad lookup, deterministic order/snapshot isolation, atomic invalid add/import, legacy boundary validation, unchanged runtime templates, all-ten composition, invalid static numbers and persisted authored template ownership/counts/defaults.

The unchanged archived M5 harness was temporarily compiled with editor-only UnrealEd/UMG dependencies, then removed; original Build.cs bytes restored. No permanent UI test dependency or helper was introduced. It verified actual panel child counts, metadata/icon resources, refresh/reconstruct binding uniqueness, teardown, native removal/stale instance rejection, possession/missing inventory and missing-owner recovery. Two audio-environment warnings occurred during that PIE run: engine 48000 versus device 192000 sample rate, and device sample-rate conversion. The test passed with no errors; these warnings were not suppressed. Final permanent suite has no warnings.

Existing build notices remain: MSVC 14.51.36257 is newer than preferred 14.50.35717; backward-compatible Unreal5_6 include-order setting. No new final source deprecation warnings remain.

## Blueprint validation

All eight compiled through MCP with `warnings_as_errors=true` after authoring and again against the final production DLL. No missing parents, unresolved nodes or compilation errors/warnings were reported.

| Blueprint | Parent | Result |
| --- | --- | --- |
| InventoryManagerComponent | /Script/NullTide.InventoryComponent | Passed |
| BP_PickUpItem | /Script/Engine.Actor | Passed |
| Item_Wood | /Script/NullTide.ItemDefinition | Passed |
| Item_Sword | /Script/NullTide.ItemDefinition | Passed |
| W_Inventory | /Script/UMG.UserWidget | Passed |
| W_InventorySlot | /Script/UMG.UserWidget | Passed |
| BP_TopDownCharacter | /Script/Engine.Character | Passed |
| BP_TopDownController | /Script/Engine.PlayerController | Passed |

Only Wood and Sword were saved. Other compiler-only dirty assets were not saved.

## PIE regression

Rendered Lvl_TopDown PIE ran through MCP/Slate in a fresh editor using command-line-only `-d3d11`; this avoids the prior M5 D3D12 timeout without changing project renderer settings. GPU-free native/M5 lifecycle tests used command-line `-nullrhi` in separate fresh editor sessions.

| Scenario | Observed result |
| --- | --- |
| Empty start | Exactly one inventory component; NativeReady; legacy mode false; empty projection; Revision 1 (M4 empty-import behavior) |
| Real Wood pickup, closed inventory | Pickup destroyed, native Wood acquired once, Revision 2 |
| Open existing UI through I | Native Wood slot with original name/description/icon |
| Real Sword pickup with same UI open | Pickup destroyed, native Sword acquired once; same open widget shows exactly two slots with green/red icons; Revision 3 |
| Identity and ordering | Two UItemInstances, GUIDs distinct, component is Outer; order [Wood, Sword] |
| Compatibility projection | InventoryItems is [Wood, Sword], matching sole native store |
| M5 UI ownership | Active W_Inventory_C_1 resolves existing controller/pawn/component; slots reference native instances and contain original metadata; legacy definition input None |
| M5 lifecycle regression | Refresh/reconstruct never duplicates slots/subscriptions; removal and missing-owner/possession recovery still pass |

Manual session GUIDs: Wood `A019D083-4A06-3AAF-BCDC-078C601A7FF1`; Sword `42B12A8D-4D0F-42CB-FF71-D2B6334BA85B`. Closed W_Inventory_C_0 and an old cleared slot were initially inspected; active widget/slot evidence is separately recorded in pie-active-widget.json. Cleared references reflect existing M5 teardown, not a gameplay failure.

No capability gameplay was invoked or added. The authored templates do not alter pickup success handling, authority, inventory events, revision, projection or UI behavior.

## Asset scope and compatibility

Before content editing: captured SHA-256 hashes for all 280 Content .uasset/.umap files. MCP class-filtered all definition/fragment assets and scanned 20 Blueprint registry entries: only Wood/Sword item definitions, no existing fragment assets. Both original fragment arrays were empty; metadata and pickup referencers captured.

Final count remains 280: exactly Wood/Sword changed, the other 278 content files byte-identical to baseline. No content added/removed/moved/renamed and no unrelated Blueprint changed. Native implementation adds 14 files and modifies six existing files, with two documentation files added. Build.cs, targets, config and uproject remain unchanged. Existing manager/pickup/UI/character/controller assets retain their original bytes.

All M1¨CM5 inventory public APIs, legacy InventoryItems/AddItem/Fragments, compatibility class and existing CoreRedirects remain. There is one native authority store, with the existing legacy projection. No authority/UI/layout cleanup or M7 work was performed.

## Remaining risks and rollback

- Concrete fragments define configuration only. Runtime equipment/combat/consumption/degradation remains future work; do not treat these templates as runtime mutable state.
- Minimal enums/types cover the prototype; additional content categories can be added when justified. New numeric configuration currently allows zero and rejects negative/nonfinite values.
- Query failure returns null/empty; use ValidateFragments/ResolveFragments diagnostic output to distinguish invalid definitions from missing capabilities.
- Existing compiler/include-order notices, map navigation diagnostics and audio sample-rate warnings are unrelated; no renderer/audio/navigation system fixes attempted. Rendered pickup validation passed with a command-line RHI override; default D3D12 behavior was not revalidated.
- The authored-content native test intentionally verifies the current Wood/Sword capability/default contract; deliberate future authored configuration changes require updating that contract.

Rollback to M5: close PIE/editor and restore the coherent six modified native files/two item assets to baseline 7f90933, remove the fourteen new M6 native files, then rebuild. Legacy Fragments/class remain available throughout. Item asset restoration should use source control/editor tooling, never handcrafted serialized bytes. No runtime inventory-to-legacy authority conversion is introduced.

## Evidence and final state

Ignored local evidence: `Saved/Validation/InventoryV2M6/`, including initial 280 hashes/definition scans/defaults, authored and fresh-reload defaults/owned template paths, successful builds, final 43-test results, archived M5 harness and combined 44-test results, eight final compile/parent results, rendered empty/Wood/Wood+Sword screenshots, runtime identities/active-widget metadata and final asset scope hashes.

Validation PIE/editor sessions closed normally; no outstanding asset edits or temporary source/dependencies. M6_PROGRESS.md records completed checkpoints and exact recovery state. Stop after M6; M7 requires a separate user instruction.
