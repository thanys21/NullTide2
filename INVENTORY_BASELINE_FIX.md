# Inventory baseline fixes — Phase 1

## Scope

Minimum native source repairs only. No Blueprint assets were edited, renamed, deleted, compiled, or saved. No ItemInstance or new InventoryComponent architecture was introduced.

## Changes

| File | Change and reason |
|---|---|
| Source/NullTide2/Content/InteractionSystem/Items/ItemDefinition.h | Added ItemDefinition.generated.h as the final include; changed export macro to NULLTIDE2_API; forward-declared UTexture2D; corrected Fragments to TArray<TObjectPtr<UInventoryItemFragment>>. These repair Unreal reflection/export requirements and replace a type trait with an object pointer. |
| Source/NullTide2/Content/InteractionSystem/Items/ItemDefinition.cpp | Uses its local ItemDefinition.h rather than an incorrect module-prefixed include path. |
| Source/NullTide2/Content/InteractionSystem/Fragments/InventoryItemFragment.h | Added InventoryItemFragment.generated.h as the final include and corrected the export macro to NULLTIDE2_API. Preserved the empty UObject subclass and existing class flags. |
| Source/NullTide2/Content/InteractionSystem/Fragments/InventoryItemFragment.cpp | Retained the matching header include; removed definitions for undeclared AInventoryItemFragment and its actor-only lifecycle. The empty UObject subclass needs no custom constructor or tick implementation. |
| Source/NullTide2/Public/InventoryItemFragment.h | Removed the duplicate, unused plain C++ class. It had no UCLASS or Blueprint exposure; source references were limited to its own declaration/implementation. |
| Source/NullTide2/Private/InventoryItemFragment.cpp | Removed the corresponding empty plain-class implementation. This also eliminates duplicate cpp basenames and competing InventoryItemFragment headers. |

## Compatibility

UItemDefinition and UInventoryItemFragment retain their reflected names, UObject parents, module ownership and class flags. ItemName, ItemDescription, ItemIcon and Fragments retain their names and property flags. No redirects, asset migrations, new runtime APIs or module dependencies were introduced. Existing item Blueprint parent references remain /Script/NullTide2.ItemDefinition.

Blueprint load/runtime compatibility has not been tested after these changes. The prior audit found that live reflection did not expose Fragments; its corrected declaration still requires a successful native build and subsequent editor reload to verify.

## Build verification — pending editor shutdown

Requested target: NullTide2Editor, Win64, Development, Unreal Engine 5.8.

```powershell
& 'D:/game/Unreal Engine/UE_5.8/Engine/Build/BatchFiles/Build.bat' NullTide2Editor Win64 Development '-Project=C:/Users/loith/Documents/Unreal Projects/NullTide2/NullTide2.uproject' -WaitMutex
```

The initial sandboxed invocation exited without useful diagnostics. The invocation outside the sandbox reached UnrealBuildTool and failed with:

> Unable to build while Live Coding is active. Exit the editor and game, or press Ctrl+Alt+F11 if iterating on code in the editor or game

Result: Failed (OtherCompilationError). This is an environment blocker, not a completed compiler validation. No claim of a successful build is made. The editor was left running to avoid disturbing unsaved work.

UBT also warned that installed MSVC 14.51.36257 is newer than its preferred 14.50.35717. No toolchain changes were made.

Next required step: close the Unreal Editor after preserving any unsaved work, rerun the command above, and fix only compiler errors within this baseline scope. Phase 1 remains incomplete until that build succeeds.
