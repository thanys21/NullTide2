#include "ItemFragment.h"

#include <initializer_list>
#include "ItemFragment_Resource.h"
#include "ItemFragment_Consumable.h"
#include "ItemFragment_Food.h"
#include "ItemFragment_Drink.h"
#include "ItemFragment_Healing.h"
#include "ItemFragment_Equippable.h"
#include "ItemFragment_Weapon.h"
#include "ItemFragment_Ammo.h"
#include "ItemFragment_Durability.h"
#include "ItemFragment_Tool.h"

namespace
{
bool ValidateNonNegative(FString& OutDiagnostic, std::initializer_list<TPair<const TCHAR*, float>> Fields)
{
	OutDiagnostic.Reset();
	for (const auto& Field : Fields)
	{
		if (!FMath::IsFinite(Field.Value) || Field.Value < 0.0f)
		{
			OutDiagnostic = FString::Printf(TEXT("%s must be finite and non-negative."), Field.Key);
			return false;
		}
	}
	return true;
}
}

bool UItemFragment_Food::ValidateTemplate(FString& OutDiagnostic) const
{
	return ValidateNonNegative(OutDiagnostic, { { TEXT("HungerRestore"), HungerRestore } });
}


bool UItemFragment_Drink::ValidateTemplate(FString& OutDiagnostic) const
{
	return ValidateNonNegative(OutDiagnostic, { { TEXT("HydrationRestore"), HydrationRestore } });
}


bool UItemFragment_Healing::ValidateTemplate(FString& OutDiagnostic) const
{
	return ValidateNonNegative(OutDiagnostic, { { TEXT("HealAmount"), HealAmount } });
}


bool UItemFragment_Weapon::ValidateTemplate(FString& OutDiagnostic) const
{
	return ValidateNonNegative(OutDiagnostic, { { TEXT("Damage"), Damage }, { TEXT("AttackSpeed"), AttackSpeed }, { TEXT("AttackRange"), AttackRange } });
}


bool UItemFragment_Ammo::ValidateTemplate(FString& OutDiagnostic) const
{
	return ValidateNonNegative(OutDiagnostic, { { TEXT("DamageModifier"), DamageModifier } });
}


bool UItemFragment_Durability::ValidateTemplate(FString& OutDiagnostic) const
{
	return ValidateNonNegative(OutDiagnostic, { { TEXT("MaxDurability"), MaxDurability } });
}


bool UItemFragment_Tool::ValidateTemplate(FString& OutDiagnostic) const
{
	return ValidateNonNegative(OutDiagnostic, { { TEXT("Efficiency"), Efficiency } });
}

