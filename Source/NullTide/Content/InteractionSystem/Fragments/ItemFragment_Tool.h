#pragma once

#include "ItemFragment.h"
#include "ItemFragmentTypes.h"
#include "ItemFragment_Tool.generated.h"

/** Static tool capability configuration; never per-instance runtime state. */
UCLASS(BlueprintType, Blueprintable, Const, DefaultToInstanced, EditInlineNew)
class NULLTIDE_API UItemFragment_Tool : public UItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tool")
	EToolType ToolType = EToolType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tool", meta = (ClampMin = "0.0"))
	float Efficiency = 1.0f;

	virtual bool ValidateTemplate(FString& OutDiagnostic) const override;
};

