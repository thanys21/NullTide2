// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Items/Fragments/ItemFragment.h"
#include "InventoryItemFragment.generated.h"

/** Legacy reflected compatibility base; new authoring uses UItemFragment and ItemFragments. */
UCLASS(Blueprintable, BlueprintType, Abstract, Const, DefaultToInstanced, EditInlineNew,
	meta = (ToolTip = "Legacy serialized fragment compatibility. Use UItemFragment and ItemFragments for new authoring."))
class NULLTIDE_API UInventoryItemFragment : public UItemFragment
{
	GENERATED_BODY()
};
