#pragma once

#include "ItemFragment.h"
#include "ItemFragment_Durability.generated.h"

/** Static durability capability configuration; never per-instance runtime state. */
UCLASS(BlueprintType, Blueprintable, Const, DefaultToInstanced, EditInlineNew)
class NULLTIDE_API UItemFragment_Durability : public UItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Durability", meta = (ClampMin = "0.0"))
	float MaxDurability = 100.0f;

	virtual bool ValidateTemplate(FString& OutDiagnostic) const override;
};

