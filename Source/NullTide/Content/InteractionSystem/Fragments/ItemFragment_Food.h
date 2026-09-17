#pragma once

#include "ItemFragment.h"
#include "ItemFragment_Food.generated.h"

/** Static food capability configuration; never per-instance runtime state. */
UCLASS(BlueprintType, Blueprintable, Const, DefaultToInstanced, EditInlineNew)
class NULLTIDE_API UItemFragment_Food : public UItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Food", meta = (ClampMin = "0.0"))
	float HungerRestore = 0.0f;

	virtual bool ValidateTemplate(FString& OutDiagnostic) const override;
};

