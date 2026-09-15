// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ItemInstance.generated.h"

class UInventoryComponent;
class UItemDefinition;

/** Runtime identity for one acquired item unit. */
UCLASS(BlueprintType)
class NULLTIDE_API UItemInstance : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Inventory|Item")
	FGuid GetInstanceId() const { return InstanceId; }

	UFUNCTION(BlueprintPure, Category = "Inventory|Item")
	TSubclassOf<UItemDefinition> GetDefinitionClass() const { return DefinitionClass; }

private:
	friend class UInventoryComponent;

	bool Initialize(const FGuid& InInstanceId, TSubclassOf<UItemDefinition> InDefinitionClass);

	UPROPERTY(VisibleInstanceOnly, Category = "Inventory|Item")
	FGuid InstanceId;

	UPROPERTY(VisibleInstanceOnly, Category = "Inventory|Item")
	TSubclassOf<UItemDefinition> DefinitionClass;
};
