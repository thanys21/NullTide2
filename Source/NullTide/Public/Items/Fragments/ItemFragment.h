// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ItemFragment.generated.h"

/** Canonical base for immutable item capability data authored on item definitions. */
UCLASS(Blueprintable, BlueprintType, Abstract, Const, DefaultToInstanced, EditInlineNew)
class NULLTIDE_API UItemFragment : public UObject
{
	GENERATED_BODY()

public:
	/** Pure static-data validation. Never mutate shared definition templates here. */
	virtual bool ValidateTemplate(FString& OutDiagnostic) const;
};
