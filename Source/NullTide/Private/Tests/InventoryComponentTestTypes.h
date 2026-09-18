// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/InventoryTypes.h"
#include "Inventory/InventoryComponent.h"
#include "Items/ItemDefinition.h"
#include "Components/ActorComponent.h"
#include "InventoryComponentTestTypes.generated.h"

class UInventoryComponent;

/** Legacy array fixture used only by native compatibility automation tests. */
UCLASS(Transient, NotBlueprintable)
class UInventoryLegacyTestManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<TSubclassOf<UItemDefinition>> InventoryItems;
};

/** Native-subclass legacy seed fixture for atomic cutover tests. */
UCLASS(Transient, NotBlueprintable)
class UInventoryCutoverTestManager : public UInventoryComponent
{
	GENERATED_BODY()
public:
	UPROPERTY()
	TArray<TSubclassOf<UItemDefinition>> InventoryItems;
};

UCLASS(Transient, NotBlueprintable)
class UInventoryOtherTestItemDefinition : public UItemDefinition
{
	GENERATED_BODY()
};

UCLASS(Transient, NotBlueprintable)
class UInventoryInvalidDataTestDefinition : public UItemDefinition
{
	GENERATED_BODY()
public:
	UInventoryInvalidDataTestDefinition() { Fragments.Add(nullptr); }
};

/** Concrete definition used only by native inventory automation tests. */
UCLASS(Transient, NotBlueprintable)
class UInventoryTestItemDefinition : public UItemDefinition
{
	GENERATED_BODY()
};

/** Dynamic delegate listener used only by native inventory automation tests. */
UCLASS(Transient, NotBlueprintable)
class UInventoryComponentTestListener : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void HandleInventoryChanged(int32 NewRevision);

	UPROPERTY()
	TObjectPtr<UInventoryComponent> Inventory;

	TSubclassOf<UItemDefinition> ReentrantDefinitionClass;
	FInventoryOperationResult ReentrantResult;
	int32 EventCount = 0;
	int32 LastRevision = 0;
	bool bAttemptReentrantMutation = false;
	bool bAttemptReentrantInitialization = false;
	FInventoryOperationResult ReentrantInitializationResult;
	int32 ObservedItemCount = 0;
	TArray<TSubclassOf<UItemDefinition>> ObservedDefinitions;
};
