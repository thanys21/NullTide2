#pragma once

#include "ItemFragment.h"
#include "ItemFragment_Resource.generated.h"

/** Static resource capability configuration; never per-instance runtime state. */
UCLASS(BlueprintType, Blueprintable, Const, DefaultToInstanced, EditInlineNew)
class NULLTIDE_API UItemFragment_Resource : public UItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Resource")
	FName ResourceType = NAME_None;
};

