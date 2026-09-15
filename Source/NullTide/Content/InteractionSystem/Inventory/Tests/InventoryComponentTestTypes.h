// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../InventoryTypes.h"
#include "../../Items/ItemDefinition.h"
#include "InventoryComponentTestTypes.generated.h"

class UInventoryComponent;

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
};
