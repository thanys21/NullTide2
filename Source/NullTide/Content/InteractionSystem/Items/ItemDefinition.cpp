// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemDefinition.h"

#include "../Fragments/InventoryItemFragment.h"

namespace
{
// Independent capabilities may share only the canonical/compatibility roots.
// Any other shared ancestor would make querying that capability ambiguous.
const UClass* SharedCapability(const UClass* First, const UClass* Second)
{
	for (const UClass* Ancestor = First; Ancestor && Ancestor != UItemFragment::StaticClass();
		Ancestor = Ancestor->GetSuperClass())
	{
		if (Ancestor != UInventoryItemFragment::StaticClass() && Second->IsChildOf(Ancestor))
		{
			return Ancestor;
		}
	}
	return nullptr;
}
}

bool UItemDefinition::ResolveFragments(TArray<UItemFragment*>& OutFragments, FString& OutDiagnostic) const
{
	OutFragments.Reset();
	OutDiagnostic.Reset();
	if (!ItemFragments.IsEmpty() && !Fragments.IsEmpty())
	{
		OutDiagnostic = TEXT("ItemFragments and legacy Fragments are both populated; choose exactly one source.");
		return false;
	}
	const bool bCanonical = !ItemFragments.IsEmpty();
	const TCHAR* Source = bCanonical ? TEXT("ItemFragments") : TEXT("Fragments");
	const int32 Count = bCanonical ? ItemFragments.Num() : Fragments.Num();
	TArray<UItemFragment*> Staged;
	Staged.Reserve(Count);
	for (int32 Index = 0; Index < Count; ++Index)
	{
		UItemFragment* Fragment = bCanonical ? ItemFragments[Index].Get() : Fragments[Index].Get();
		if (!IsValid(Fragment) || Fragment->GetClass()->HasAnyClassFlags(CLASS_Deprecated | CLASS_NewerVersionExists))
		{
			OutDiagnostic = FString::Printf(TEXT("%s[%d] is null or invalid."), Source, Index);
			return false;
		}
		for (int32 Earlier = 0; Earlier < Staged.Num(); ++Earlier)
		{
			if (const UClass* Capability = SharedCapability(Staged[Earlier]->GetClass(), Fragment->GetClass()))
			{
				OutDiagnostic = FString::Printf(TEXT("%s[%d] and %s[%d] duplicate capability %s (ambiguous subclass match)."),
					Source, Earlier, Source, Index, *Capability->GetPathName());
				return false;
			}
		}
		FString TemplateDiagnostic;
		if (!Fragment->ValidateTemplate(TemplateDiagnostic))
		{
			OutDiagnostic = FString::Printf(TEXT("%s[%d] (%s): %s"), Source, Index,
				*Fragment->GetClass()->GetPathName(), *TemplateDiagnostic);
			return false;
		}
		Staged.Add(Fragment);
	}
	OutFragments = MoveTemp(Staged);
	return true;
}

bool UItemDefinition::ValidateFragments(FString& OutDiagnostic) const
{
	TArray<UItemFragment*> Resolved;
	return ResolveFragments(Resolved, OutDiagnostic);
}

TArray<UItemFragment*> UItemDefinition::GetResolvedFragments() const
{
	TArray<UItemFragment*> Resolved;
	FString Diagnostic;
	ResolveFragments(Resolved, Diagnostic);
	return Resolved;
}

UItemFragment* UItemDefinition::FindFragmentByClass(TSubclassOf<UItemFragment> FragmentClass) const
{
	const UClass* QueryClass = FragmentClass.Get();
	if (!QueryClass || QueryClass->HasAnyClassFlags(CLASS_Deprecated | CLASS_NewerVersionExists)) { return nullptr; }
	TArray<UItemFragment*> Resolved;
	FString Diagnostic;
	if (!ResolveFragments(Resolved, Diagnostic)) { return nullptr; }
	UItemFragment* Match = nullptr;
	for (UItemFragment* Fragment : Resolved)
	{
		if (Fragment->IsA(QueryClass))
		{
			if (Match) { return nullptr; }
			Match = Fragment;
		}
	}
	return Match;
}

bool UItemDefinition::HasFragmentByClass(TSubclassOf<UItemFragment> FragmentClass) const
{
	return FindFragmentByClass(FragmentClass) != nullptr;
}

