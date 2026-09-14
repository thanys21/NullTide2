// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ItemDefinition.generated.h"

class UInventoryItemFragment;
class UTexture2D;
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, Abstract, Const)
class NULLTIDE2_API UItemDefinition : public UObject
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Display")
	FText ItemName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Display")
	FText ItemDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Display")
	TObjectPtr<UTexture2D> ItemIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fragments Array")
	TArray<TObjectPtr<UInventoryItemFragment>> Fragments;
};
