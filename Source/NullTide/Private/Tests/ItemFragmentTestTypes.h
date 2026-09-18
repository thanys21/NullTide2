#pragma once

#include "Compatibility/InventoryItemFragment.h"
#include "Items/Fragments/ItemFragment_Resource.h"
#include "Items/ItemDefinition.h"
#include "ItemFragmentTestTypes.generated.h"

UCLASS(Transient, NotBlueprintable)
class UItemFragmentLegacyTestCapability : public UInventoryItemFragment
{
	GENERATED_BODY()
};

UCLASS(Transient, NotBlueprintable)
class UItemFragmentResourceTestChild : public UItemFragment_Resource
{
	GENERATED_BODY()
};

UCLASS(Transient, NotBlueprintable)
class UItemFragmentResourceTestSibling : public UItemFragment_Resource
{
	GENERATED_BODY()
};

UCLASS(Transient, NotBlueprintable)
class UInventoryCanonicalInvalidTestDefinition : public UItemDefinition
{
	GENERATED_BODY()
public:
	UInventoryCanonicalInvalidTestDefinition() { ItemFragments.Add(nullptr); }
};

UCLASS(Transient, NotBlueprintable)
class UInventoryFragmentTemplateTestDefinition : public UItemDefinition
{
	GENERATED_BODY()
public:
	UInventoryFragmentTemplateTestDefinition();
};
