#pragma once

#include "ItemFragment.h"
#include "ItemFragmentTypes.h"
#include "ItemFragment_Ammo.generated.h"

/** Static ammo capability configuration; never per-instance runtime state. */
UCLASS(BlueprintType, Blueprintable, Const, DefaultToInstanced, EditInlineNew)
class NULLTIDE_API UItemFragment_Ammo : public UItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo")
	EAmmoType AmmoType = EAmmoType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo", meta = (ClampMin = "0.0"))
	float DamageModifier = 1.0f;

	virtual bool ValidateTemplate(FString& OutDiagnostic) const override;
};

