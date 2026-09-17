#pragma once

#include "ItemFragment.h"
#include "ItemFragment_Drink.generated.h"

/** Static drink capability configuration; never per-instance runtime state. */
UCLASS(BlueprintType, Blueprintable, Const, DefaultToInstanced, EditInlineNew)
class NULLTIDE_API UItemFragment_Drink : public UItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drink", meta = (ClampMin = "0.0"))
	float HydrationRestore = 0.0f;

	virtual bool ValidateTemplate(FString& OutDiagnostic) const override;
};

