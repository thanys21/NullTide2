// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

class UInventoryItemFragment;
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, Abstract, Const)
class INVENTORYSYSTEM_API UItemDefinition : public UObject
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
	TArray<TIsTObjectPtr<UInventoryItemFragment>> Fragments;
};
