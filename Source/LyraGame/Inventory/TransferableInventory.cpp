// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/TransferableInventory.h"

#include "LyraGameplayTags.h"
#include "LyraInventoryItemInstance.h"
#include "GameFramework/GameplayMessageSubsystem.h"

// Add default functionality here for any ITransferableInventory functions that are not pure virtual.
void ITransferableInventory::TransferSlots_Implementation(FTransferInventoryData Data)
{
	if(Data.bTransferred) return;
	
	Data.SourceItem->Swap(Data.DestItem);
	Data.bTransferred = true;
	Execute_TransferSlots(Data.SourceInventory, Data);
}

TArray<ULyraInventoryItemInstance*> ITransferableInventory::GetAllItems_Implementation()
{
	return TArray<ULyraInventoryItemInstance*>();
}
