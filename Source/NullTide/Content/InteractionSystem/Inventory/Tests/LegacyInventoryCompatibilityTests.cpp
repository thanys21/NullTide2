// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryComponentTestTypes.h"

#include "../InventoryComponent.h"
#include "../LegacyInventoryCompatibilityLibrary.h"
#include "Misc/AutomationTest.h"
#include "UObject/StrongObjectPtr.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FLegacyInventoryValidAddTest,
	"NullTide.Inventory.M2.LegacyValidAdd",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FLegacyInventoryValidAddTest::RunTest(const FString& Parameters)
{
	const TStrongObjectPtr<UInventoryLegacyTestManager> Manager(
		NewObject<UInventoryLegacyTestManager>(GetTransientPackage()));
	const FInventoryOperationResult Result =
		ULegacyInventoryCompatibilityLibrary::TryAddDefinitionToLegacyInventory(
			Manager.Get(), UInventoryTestItemDefinition::StaticClass());

	TestTrue(TEXT("Valid legacy add succeeds"), Result.IsSuccess());
	TestEqual(TEXT("Exactly one class is appended"), Manager->InventoryItems.Num(), 1);
	TestEqual(TEXT("Appended class matches the definition"), Manager->InventoryItems[0].Get(), UInventoryTestItemDefinition::StaticClass());
	TestNull(TEXT("Legacy add does not create an item instance"), Result.Item.Get());
	TestFalse(TEXT("Legacy add does not create an item ID"), Result.ItemId.IsValid());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FLegacyInventoryInvalidDefinitionTest,
	"NullTide.Inventory.M2.LegacyInvalidDefinitionDoesNotMutate",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FLegacyInventoryInvalidDefinitionTest::RunTest(const FString& Parameters)
{
	const TStrongObjectPtr<UInventoryLegacyTestManager> Manager(
		NewObject<UInventoryLegacyTestManager>(GetTransientPackage()));
	const FInventoryOperationResult Result =
		ULegacyInventoryCompatibilityLibrary::TryAddDefinitionToLegacyInventory(Manager.Get(), nullptr);

	TestTrue(TEXT("Null definition is rejected"), Result.Result == EInventoryOperationResult::InvalidDefinition);
	TestEqual(TEXT("Invalid definition does not mutate the array"), Manager->InventoryItems.Num(), 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FLegacyInventoryInvalidReceiverTest,
	"NullTide.Inventory.M2.LegacyInvalidReceiverDoesNotMutate",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FLegacyInventoryInvalidReceiverTest::RunTest(const FString& Parameters)
{
	const TStrongObjectPtr<UInventoryComponent> NotALegacyInventory(
		NewObject<UInventoryComponent>(GetTransientPackage()));
	const FInventoryOperationResult Result =
		ULegacyInventoryCompatibilityLibrary::TryAddDefinitionToLegacyInventory(
			NotALegacyInventory.Get(), UInventoryTestItemDefinition::StaticClass());

	TestTrue(TEXT("Component without the legacy boundary is rejected"), Result.Result == EInventoryOperationResult::NotInitialized);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FLegacyInventorySnapshotTest,
	"NullTide.Inventory.M2.LegacySnapshotIsIsolated",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FLegacyInventorySnapshotTest::RunTest(const FString& Parameters)
{
	const TStrongObjectPtr<UInventoryLegacyTestManager> Manager(
		NewObject<UInventoryLegacyTestManager>(GetTransientPackage()));
	ULegacyInventoryCompatibilityLibrary::TryAddDefinitionToLegacyInventory(
		Manager.Get(), UInventoryTestItemDefinition::StaticClass());

	EInventoryOperationResult QueryResult = EInventoryOperationResult::NotInitialized;
	TArray<TSubclassOf<UItemDefinition>> Snapshot =
		ULegacyInventoryCompatibilityLibrary::GetLegacyItemsSnapshot(Manager.Get(), QueryResult);
	Snapshot.Reset();

	TestTrue(TEXT("Legacy query succeeds"), QueryResult == EInventoryOperationResult::Success);
	TestEqual(TEXT("Editing snapshot does not mutate authoritative array"), Manager->InventoryItems.Num(), 1);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
