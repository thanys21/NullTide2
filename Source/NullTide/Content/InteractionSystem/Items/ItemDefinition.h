// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ItemDefinition.generated.h"

class UInventoryItemFragment;
class UItemFragment;
class UTexture2D;
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, Abstract, Const)
class NULLTIDE_API UItemDefinition : public UObject
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

	/** Static capability templates. Use one source only; Fragments is legacy fallback. */
	UPROPERTY(Instanced, EditDefaultsOnly, BlueprintReadOnly, Category = "Items|Fragments",
		meta = (ToolTip = "Canonical static capability templates. Leave legacy Fragments empty."))
	TArray<TObjectPtr<UItemFragment>> ItemFragments;

	/** Validated snapshot; failure returns no fragments and an indexed diagnostic. */
	UFUNCTION(BlueprintPure, Category = "Items|Fragments")
	bool ResolveFragments(TArray<UItemFragment*>& OutFragments, FString& OutDiagnostic) const;

	UFUNCTION(BlueprintPure, Category = "Items|Fragments")
	bool ValidateFragments(FString& OutDiagnostic) const;

	/** Returns a copy of the validated source, or an empty array on invalid data. */
	UFUNCTION(BlueprintPure, Category = "Items|Fragments")
	TArray<UItemFragment*> GetResolvedFragments() const;

	/** Returns no match for invalid definitions, invalid query classes or ambiguity. */
	UFUNCTION(BlueprintPure, Category = "Items|Fragments", meta = (DeterminesOutputType = "FragmentClass"))
	UItemFragment* FindFragmentByClass(TSubclassOf<UItemFragment> FragmentClass) const;

	UFUNCTION(BlueprintPure, Category = "Items|Fragments")
	bool HasFragmentByClass(TSubclassOf<UItemFragment> FragmentClass) const;
};
