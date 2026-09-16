// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryComponent.h"

#include "../Fragments/InventoryItemFragment.h"
#include "../Items/ItemDefinition.h"
#include "../Items/ItemInstance.h"

namespace
{
FInventoryOperationResult MakeInventoryResult(
	EInventoryOperationResult Result,
	UItemInstance* Item = nullptr,
	const FGuid& ItemId = FGuid())
{
	FInventoryOperationResult OperationResult;
	OperationResult.Result = Result;
	OperationResult.Item = Item;
	OperationResult.ItemId = ItemId;
	return OperationResult;
}
}

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

FInventoryOperationResult UInventoryComponent::TryAddDefinition(
	TSubclassOf<UItemDefinition> DefinitionClass)
{
	if (!CanOperate())
	{
		return MakeInventoryResult(EInventoryOperationResult::NotInitialized);
	}

	if (bMutationInProgress)
	{
		return MakeInventoryResult(EInventoryOperationResult::Busy);
	}

	TGuardValue<bool> MutationGuard(bMutationInProgress, true);
	UClass* DefinitionUClass = DefinitionClass.Get();
	if (!DefinitionUClass || DefinitionUClass->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists))
	{
		return MakeInventoryResult(EInventoryOperationResult::InvalidDefinition);
	}

	const UItemDefinition* Definition = Cast<UItemDefinition>(DefinitionUClass->GetDefaultObject());
	if (!Definition)
	{
		return MakeInventoryResult(EInventoryOperationResult::InvalidDefinition);
	}

	if (!IsDefinitionDataValid(Definition))
	{
		return MakeInventoryResult(EInventoryOperationResult::InvalidDefinitionData);
	}

	FGuid NewItemId;
	do
	{
		NewItemId = FGuid::NewGuid();
	}
	while (!NewItemId.IsValid() || HasItemId(NewItemId));

	UItemInstance* NewItem = NewObject<UItemInstance>(this);
	if (!NewItem || !NewItem->Initialize(NewItemId, DefinitionClass))
	{
		return MakeInventoryResult(EInventoryOperationResult::InvalidDefinitionData);
	}

	Items.Add(NewItem);
	++Revision;

	const FInventoryOperationResult Result = MakeInventoryResult(
		EInventoryOperationResult::Success,
		NewItem,
		NewItemId);
	OnInventoryChanged.Broadcast(Revision);
	return Result;
}

FInventoryOperationResult UInventoryComponent::TryRemoveItem(FGuid ItemId)
{
	if (!CanOperate())
	{
		return MakeInventoryResult(EInventoryOperationResult::NotInitialized, nullptr, ItemId);
	}

	if (bMutationInProgress)
	{
		return MakeInventoryResult(EInventoryOperationResult::Busy, nullptr, ItemId);
	}

	TGuardValue<bool> MutationGuard(bMutationInProgress, true);
	if (!ItemId.IsValid())
	{
		return MakeInventoryResult(EInventoryOperationResult::InvalidItemId, nullptr, ItemId);
	}

	const int32 ItemIndex = Items.IndexOfByPredicate(
		[&ItemId](const TObjectPtr<UItemInstance>& Item)
		{
			return IsValid(Item) && Item->GetInstanceId() == ItemId;
		});

	if (ItemIndex == INDEX_NONE)
	{
		return MakeInventoryResult(EInventoryOperationResult::InvalidItemId, nullptr, ItemId);
	}

	UItemInstance* RemovedItem = Items[ItemIndex];
	Items.RemoveAt(ItemIndex);
	++Revision;

	const FInventoryOperationResult Result = MakeInventoryResult(
		EInventoryOperationResult::Success,
		RemovedItem,
		ItemId);
	OnInventoryChanged.Broadcast(Revision);
	return Result;
}

TArray<UItemInstance*> UInventoryComponent::GetItemsSnapshot() const
{
	TArray<UItemInstance*> Snapshot;
	Snapshot.Reserve(Items.Num());
	for (UItemInstance* Item : Items)
	{
		Snapshot.Add(Item);
	}
	return Snapshot;
}

UItemInstance* UInventoryComponent::FindItemById(FGuid ItemId) const
{
	if (!ItemId.IsValid())
	{
		return nullptr;
	}

	const TObjectPtr<UItemInstance>* FoundItem = Items.FindByPredicate(
		[&ItemId](const TObjectPtr<UItemInstance>& Item)
		{
			return IsValid(Item) && Item->GetInstanceId() == ItemId;
		});
	return FoundItem ? FoundItem->Get() : nullptr;
}

bool UInventoryComponent::ContainsItem(FGuid ItemId) const
{
	return FindItemById(ItemId) != nullptr;
}

bool UInventoryComponent::CanOperate() const
{
	return !bLegacyInventoryMode && !HasAnyFlags(RF_ClassDefaultObject | RF_BeginDestroyed | RF_FinishDestroyed);
}

bool UInventoryComponent::IsDefinitionDataValid(const UItemDefinition* Definition) const
{
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

bool UInventoryComponent::HasItemId(const FGuid& ItemId) const
{
	return Items.ContainsByPredicate(
		[&ItemId](const TObjectPtr<UItemInstance>& Item)
		{
			return IsValid(Item) && Item->GetInstanceId() == ItemId;
		});
}
