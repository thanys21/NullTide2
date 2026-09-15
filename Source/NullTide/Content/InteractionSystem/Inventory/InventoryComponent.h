// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryTypes.h"
#include "InventoryComponent.generated.h"

class UItemDefinition;
class UItemInstance;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryChanged, int32, NewRevision);

/** Owns item membership and all inventory mutations. */
UCLASS(ClassGroup = (Inventory), meta = (BlueprintSpawnableComponent))
class NULLTIDE_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	FInventoryOperationResult TryAddDefinition(TSubclassOf<UItemDefinition> DefinitionClass);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	FInventoryOperationResult TryRemoveItem(FGuid ItemId);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	TArray<UItemInstance*> GetItemsSnapshot() const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	UItemInstance* FindItemById(FGuid ItemId) const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool ContainsItem(FGuid ItemId) const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetItemCount() const { return Items.Num(); }

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetRevision() const { return Revision; }

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryChanged OnInventoryChanged;

private:
	bool CanOperate() const;
	bool IsDefinitionDataValid(const UItemDefinition* Definition) const;
	bool HasItemId(const FGuid& ItemId) const;

	UPROPERTY()
	TArray<TObjectPtr<UItemInstance>> Items;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	int32 Revision = 0;

	bool bMutationInProgress = false;
};
