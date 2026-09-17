#pragma once

#include "ItemFragment.h"
#include "ItemFragmentTypes.h"
#include "ItemFragment_Equippable.generated.h"

/** Static equippable capability configuration; never per-instance runtime state. */
UCLASS(BlueprintType, Blueprintable, Const, DefaultToInstanced, EditInlineNew)
class NULLTIDE_API UItemFragment_Equippable : public UItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equippable")
	EEquipmentSlot EquipmentSlot = EEquipmentSlot::None;
};

