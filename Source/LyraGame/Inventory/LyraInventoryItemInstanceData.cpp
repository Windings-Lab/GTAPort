// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/LyraInventoryItemInstanceData.h"

#include "Net/UnrealNetwork.h"

void ULyraInventoryItemInstanceData::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, StatTags);
	DOREPLIFETIME(ThisClass, ItemDef);
	DOREPLIFETIME(ThisClass, ItemCount);
	DOREPLIFETIME(ThisClass, LastItemCount);
}
