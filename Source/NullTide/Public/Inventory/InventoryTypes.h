// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InventoryTypes.generated.h"

class UItemInstance;

UENUM(BlueprintType)
enum class EInventoryInitializationState : uint8
{
	NotRequired,
	PendingLegacyImport,
	NativeReady,
	Failed
};

UENUM(BlueprintType, meta = (ScriptName = "InventoryOperationResultCode"))
enum class EInventoryOperationResult : uint8
{
	Success,
	InvalidDefinition,
	InvalidDefinitionData,
	InvalidItemId,
	NotInitialized,
	Busy
};

USTRUCT(BlueprintType)
struct NULLTIDE_API FInventoryOperationResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	EInventoryOperationResult Result = EInventoryOperationResult::NotInitialized;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<UItemInstance> Item = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	FGuid ItemId;

	bool IsSuccess() const
	{
		return Result == EInventoryOperationResult::Success;
	}
};

/** Blueprint helpers for inventory operation results. */
UCLASS()
class NULLTIDE_API UInventoryOperationResultLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Inventory", meta = (DisplayName = "Is Success", ScriptMethod))
	static bool IsSuccess(const FInventoryOperationResult& OperationResult)
	{
		return OperationResult.IsSuccess();
	}
};
