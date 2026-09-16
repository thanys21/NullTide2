// Fill out your copyright notice in the Description page of Project Settings.

#include "LegacyInventoryCompatibilityLibrary.h"

#include "../Fragments/InventoryItemFragment.h"
#include "../Items/ItemDefinition.h"
#include "Components/ActorComponent.h"
#include "Misc/ScopeExit.h"
#include "UObject/UnrealType.h"

namespace
{
const FName LegacyItemsPropertyName(TEXT("InventoryItems"));
TSet<TWeakObjectPtr<UActorComponent>> ActiveLegacyMutations;

FInventoryOperationResult MakeLegacyResult(EInventoryOperationResult Result)
{
	FInventoryOperationResult OperationResult;
	OperationResult.Result = Result;
	return OperationResult;
}

bool IsLegacyReceiverUsable(const UActorComponent* LegacyInventory)
{
	return IsValid(LegacyInventory)
		&& !LegacyInventory->HasAnyFlags(RF_ClassDefaultObject | RF_BeginDestroyed | RF_FinishDestroyed);
}

bool IsDefinitionUsable(TSubclassOf<UItemDefinition> DefinitionClass)
{
	UClass* DefinitionUClass = DefinitionClass.Get();
	return DefinitionUClass
		&& !DefinitionUClass->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists);
}

bool IsDefinitionDataValid(TSubclassOf<UItemDefinition> DefinitionClass)
{
	const UClass* DefinitionUClass = DefinitionClass.Get();
	const UItemDefinition* Definition = DefinitionUClass
		? Cast<UItemDefinition>(DefinitionUClass->GetDefaultObject())
		: nullptr;
	if (!IsValid(Definition))
	{
		return false;
	}

	for (const UInventoryItemFragment* Fragment : Definition->Fragments)
	{
		if (!IsValid(Fragment))
		{
			return false;
		}
	}
	return true;
}

bool FindLegacyArray(
	UActorComponent* LegacyInventory,
	FArrayProperty*& OutArrayProperty,
	FClassProperty*& OutClassProperty)
{
	OutArrayProperty = FindFProperty<FArrayProperty>(LegacyInventory->GetClass(), LegacyItemsPropertyName);
	OutClassProperty = OutArrayProperty ? CastField<FClassProperty>(OutArrayProperty->Inner) : nullptr;
	return OutClassProperty
		&& OutClassProperty->MetaClass
		&& OutClassProperty->MetaClass->IsChildOf(UItemDefinition::StaticClass());
}
}

FInventoryOperationResult ULegacyInventoryCompatibilityLibrary::TryAddDefinitionToLegacyInventory(
	UActorComponent* LegacyInventory,
	TSubclassOf<UItemDefinition> DefinitionClass)
{
	if (!IsLegacyReceiverUsable(LegacyInventory))
	{
		return MakeLegacyResult(EInventoryOperationResult::NotInitialized);
	}

	if (ActiveLegacyMutations.Contains(LegacyInventory))
	{
		return MakeLegacyResult(EInventoryOperationResult::Busy);
	}

	if (!IsDefinitionUsable(DefinitionClass))
	{
		return MakeLegacyResult(EInventoryOperationResult::InvalidDefinition);
	}

	if (!IsDefinitionDataValid(DefinitionClass))
	{
		return MakeLegacyResult(EInventoryOperationResult::InvalidDefinitionData);
	}

	FArrayProperty* ArrayProperty = nullptr;
	FClassProperty* ClassProperty = nullptr;
	if (!FindLegacyArray(LegacyInventory, ArrayProperty, ClassProperty)
		|| !DefinitionClass->IsChildOf(ClassProperty->MetaClass))
	{
		return MakeLegacyResult(EInventoryOperationResult::NotInitialized);
	}

	ActiveLegacyMutations.Add(LegacyInventory);
	ON_SCOPE_EXIT
	{
		ActiveLegacyMutations.Remove(LegacyInventory);
	};

	FScriptArrayHelper Items(ArrayProperty, ArrayProperty->ContainerPtrToValuePtr<void>(LegacyInventory));
	const int32 NewIndex = Items.AddValue();
	ClassProperty->SetObjectPropertyValue(Items.GetRawPtr(NewIndex), DefinitionClass.Get());
	return MakeLegacyResult(EInventoryOperationResult::Success);
}

TArray<TSubclassOf<UItemDefinition>> ULegacyInventoryCompatibilityLibrary::GetLegacyItemsSnapshot(
	UActorComponent* LegacyInventory,
	EInventoryOperationResult& Result)
{
	TArray<TSubclassOf<UItemDefinition>> Snapshot;
	if (!IsLegacyReceiverUsable(LegacyInventory))
	{
		Result = EInventoryOperationResult::NotInitialized;
		return Snapshot;
	}

	FArrayProperty* ArrayProperty = nullptr;
	FClassProperty* ClassProperty = nullptr;
	if (!FindLegacyArray(LegacyInventory, ArrayProperty, ClassProperty))
	{
		Result = EInventoryOperationResult::NotInitialized;
		return Snapshot;
	}

	FScriptArrayHelper Items(ArrayProperty, ArrayProperty->ContainerPtrToValuePtr<void>(LegacyInventory));
	Snapshot.Reserve(Items.Num());
	for (int32 Index = 0; Index < Items.Num(); ++Index)
	{
		Snapshot.Add(Cast<UClass>(ClassProperty->GetObjectPropertyValue(Items.GetRawPtr(Index))));
	}

	Result = EInventoryOperationResult::Success;
	return Snapshot;
}
