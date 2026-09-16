// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LegacyInventoryCompatibilityLibrary.generated.h"

class UActorComponent;
class UItemDefinition;

/** Compatibility boundary: legacy rollback writes before cutover, native transactions/queries afterward. */
UCLASS()
class NULLTIDE_API ULegacyInventoryCompatibilityLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory|Compatibility")
	static FInventoryOperationResult TryAddDefinitionToLegacyInventory(
		UActorComponent* LegacyInventory,
		TSubclassOf<UItemDefinition> DefinitionClass);

	UFUNCTION(BlueprintPure, Category = "Inventory|Compatibility")
	static TArray<TSubclassOf<UItemDefinition>> GetLegacyItemsSnapshot(
		UActorComponent* LegacyInventory,
		EInventoryOperationResult& Result);
};
