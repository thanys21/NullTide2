# Null Tide

Unreal Engine 5.8 project.

## AI Development Rules

- Inspect existing architecture before modifying it.
- Never modify .uasset files directly.
- Use Unreal MCP for Unreal assets and Blueprints.
- Do not delete or rename Blueprint-exposed C++ APIs without checking references.
- Compile after C++ changes.
- Compile affected Blueprints after Blueprint changes.
- Prefer composition over inheritance for inventory items.
- Static item data belongs in ItemDefinition.
- Runtime item state belongs in ItemInstance.
- Inventory logic belongs in InventoryComponent.