// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryComponent.h"

#include "LegacyInventoryStorage.h"
#include "UObject/StrongObjectPtr.h"

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

void UInventoryComponent::BeginPlay()
{
	if (bImportLegacyInventoryOnBeginPlay)
	{
		const FInventoryOperationResult Result = InitializeFromLegacyInventory();
		if (!Result.IsSuccess())
		{
			UE_LOG(LogTemp, Warning, TEXT("Inventory seed import failed for %s (result %d); writes remain blocked."),
				*GetPathName(), static_cast<int32>(Result.Result));
		}
	}
	Super::BeginPlay();
}

EInventoryInitializationState UInventoryComponent::GetInitializationState() const
{
	if (InitializationState == EInventoryInitializationState::NotRequired
		&& (bLegacyInventoryMode || bImportLegacyInventoryOnBeginPlay))
	{
		return EInventoryInitializationState::PendingLegacyImport;
	}
	return InitializationState;
}

FInventoryOperationResult UInventoryComponent::InitializeFromLegacyInventory()
{
	if (HasAnyFlags(RF_ClassDefaultObject | RF_BeginDestroyed | RF_FinishDestroyed))
	{
		return MakeInventoryResult(EInventoryOperationResult::NotInitialized);
	}
	if (bMutationInProgress)
	{
		return MakeInventoryResult(EInventoryOperationResult::Busy);
	}
	if (InitializationState == EInventoryInitializationState::NativeReady)
	{
		return MakeInventoryResult(EInventoryOperationResult::Success);
	}
	if (InitializationState == EInventoryInitializationState::Failed)
	{
		return MakeInventoryResult(LastInitializationResult);
	}
	if (!bLegacyInventoryMode)
	{
		return MakeInventoryResult(EInventoryOperationResult::NotInitialized);
	}

	TGuardValue<bool> MutationGuard(bMutationInProgress, true);
	auto FailImport = [this](EInventoryOperationResult Result)
	{
		InitializationState = EInventoryInitializationState::Failed;
		LastInitializationResult = Result;
		return MakeInventoryResult(Result);
	};
	FArrayProperty* ArrayProperty = nullptr;
	FClassProperty* ClassProperty = nullptr;
	if (!Items.IsEmpty() || Revision != 0
		|| !LegacyInventoryStorage::FindArray(this, ArrayProperty, ClassProperty))
	{
		return FailImport(EInventoryOperationResult::NotInitialized);
	}
	const TArray<TSubclassOf<UItemDefinition>> Seed =
		LegacyInventoryStorage::ReadArray(this, ArrayProperty, ClassProperty);
	for (const TSubclassOf<UItemDefinition>& DefinitionClass : Seed)
	{
		UClass* Definition = DefinitionClass.Get();
		if (!Definition || Definition->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists)
			|| !Definition->IsChildOf(ClassProperty->MetaClass))
		{
			return FailImport(EInventoryOperationResult::InvalidDefinition);
		}
		if (!IsDefinitionDataValid(Cast<UItemDefinition>(Definition->GetDefaultObject())))
		{
			return FailImport(EInventoryOperationResult::InvalidDefinitionData);
		}
	}

	TArray<TStrongObjectPtr<UItemInstance>> StagedReferences;
	TArray<TObjectPtr<UItemInstance>> ImportedItems;
	TSet<FGuid> ImportedIds;
	ImportedItems.Reserve(Seed.Num());
	StagedReferences.Reserve(Seed.Num());
	for (const TSubclassOf<UItemDefinition>& DefinitionClass : Seed)
	{
		FGuid ItemId;
		do { ItemId = FGuid::NewGuid(); } while (!ItemId.IsValid() || ImportedIds.Contains(ItemId));
		UItemInstance* Item = NewObject<UItemInstance>(this);
		StagedReferences.Emplace(Item);
		if (!Item || !Item->Initialize(ItemId, DefinitionClass))
		{
			return FailImport(EInventoryOperationResult::InvalidDefinitionData);
		}
		ImportedIds.Add(ItemId);
		ImportedItems.Add(Item);
	}

	Items = MoveTemp(ImportedItems);
	RebuildLegacyProjection(ArrayProperty, ClassProperty);
	bLegacyInventoryMode = false;
	InitializationState = EInventoryInitializationState::NativeReady;
	LastInitializationResult = EInventoryOperationResult::Success;
	++Revision;
	OnInventoryChanged.Broadcast(Revision);
	return MakeInventoryResult(EInventoryOperationResult::Success);
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

	FArrayProperty* ProjectionArray = nullptr;
	FClassProperty* ProjectionClass = nullptr;
	if (InitializationState == EInventoryInitializationState::NativeReady)
	{
		if (!LegacyInventoryStorage::FindArray(this, ProjectionArray, ProjectionClass))
		{
			return MakeInventoryResult(EInventoryOperationResult::NotInitialized);
		}
		if (!DefinitionUClass->IsChildOf(ProjectionClass->MetaClass))
		{
			return MakeInventoryResult(EInventoryOperationResult::InvalidDefinition);
		}
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
	if (ProjectionArray)
	{
		RebuildLegacyProjection(ProjectionArray, ProjectionClass);
	}
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

	FArrayProperty* ProjectionArray = nullptr;
	FClassProperty* ProjectionClass = nullptr;
	if (InitializationState == EInventoryInitializationState::NativeReady
		&& !LegacyInventoryStorage::FindArray(this, ProjectionArray, ProjectionClass))
	{
		return MakeInventoryResult(EInventoryOperationResult::NotInitialized, nullptr, ItemId);
	}

	UItemInstance* RemovedItem = Items[ItemIndex];
	Items.RemoveAt(ItemIndex);
	if (ProjectionArray)
	{
		RebuildLegacyProjection(ProjectionArray, ProjectionClass);
	}
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
	return !bLegacyInventoryMode
		&& (!bImportLegacyInventoryOnBeginPlay || InitializationState == EInventoryInitializationState::NativeReady)
		&& InitializationState != EInventoryInitializationState::Failed
		&& !HasAnyFlags(RF_ClassDefaultObject | RF_BeginDestroyed | RF_FinishDestroyed);
}

void UInventoryComponent::RebuildLegacyProjection(FArrayProperty* ArrayProperty, FClassProperty* ClassProperty)
{
	TArray<TSubclassOf<UItemDefinition>> Definitions;
	Definitions.Reserve(Items.Num());
	for (const UItemInstance* Item : Items)
	{
		Definitions.Add(Item->GetDefinitionClass());
	}
	LegacyInventoryStorage::WriteArray(this, ArrayProperty, ClassProperty, Definitions);
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
