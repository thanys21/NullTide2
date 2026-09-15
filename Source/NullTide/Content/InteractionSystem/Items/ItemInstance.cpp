// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemInstance.h"

#include "ItemDefinition.h"

bool UItemInstance::Initialize(
	const FGuid& InInstanceId,
	TSubclassOf<UItemDefinition> InDefinitionClass)
{
	if (InstanceId.IsValid() || DefinitionClass || !InInstanceId.IsValid() || !InDefinitionClass)
	{
		return false;
	}

	InstanceId = InInstanceId;
	DefinitionClass = InDefinitionClass;
	return true;
}
