#pragma once

#include "Items/Fragments/ItemFragment.h"
#include "ItemFragment_Consumable.generated.h"

/** Static consumable capability configuration; never per-instance runtime state. */
UCLASS(BlueprintType, Blueprintable, Const, DefaultToInstanced, EditInlineNew)
class NULLTIDE_API UItemFragment_Consumable : public UItemFragment
{
	GENERATED_BODY()

public:
	// Marker capability. Use behavior/runtime state is deliberately deferred.
};

