#pragma once

#include "Items/Fragments/ItemFragment.h"
#include "ItemFragment_Healing.generated.h"

/** Static healing capability configuration; never per-instance runtime state. */
UCLASS(BlueprintType, Blueprintable, Const, DefaultToInstanced, EditInlineNew)
class NULLTIDE_API UItemFragment_Healing : public UItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Healing", meta = (ClampMin = "0.0"))
	float HealAmount = 0.0f;

	virtual bool ValidateTemplate(FString& OutDiagnostic) const override;
};

