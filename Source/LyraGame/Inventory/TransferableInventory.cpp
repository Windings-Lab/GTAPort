// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/TransferableInventory.h"

#include "LyraGameplayTags.h"
#include "LyraInventoryItemInstance.h"
#include "GameFramework/GameplayMessageSubsystem.h"

// Add default functionality here for any ITransferableInventory functions that are not pure virtual.
void ITransferableInventory::TransferSlots_Implementation(UObject* WorldContextObject, FTransferInventoryData Data)
{
	Data.SourceItem->Swap(Data.DestItem);
}

TArray<ULyraInventoryItemInstance*> ITransferableInventory::GetAllItems_Implementation()
{
	return TArray<ULyraInventoryItemInstance*>();
}
