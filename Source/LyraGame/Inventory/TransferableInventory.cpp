// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/TransferableInventory.h"

#include "LyraGameplayTags.h"
#include "LyraInventoryItemInstance.h"
#include "GameFramework/GameplayMessageSubsystem.h"

// Add default functionality here for any ITransferableInventory functions that are not pure virtual.
void ITransferableInventory::TransferSlots_Implementation(UObject* WorldContextObject, FTransferInventoryData Data)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return;
	}

	Data.SourceItem->Swap(Data.DestItem);

	FSlotChangedMessage SlotChanged;
	SlotChanged.Index = Data.DestIndex;
	SlotChanged.Item  = Data.DestItem;
	OnTransferSlotsFinished_Implementation(World, SlotChanged);
	
	SlotChanged.Index = Data.SourceIndex;
	SlotChanged.Item  = Data.SourceItem;
	Execute_OnTransferSlotsFinished(Data.SourceInventory, World, SlotChanged);
}

void ITransferableInventory::OnTransferSlotsFinished_Implementation(const UWorld* World, FSlotChangedMessage Message)
{
	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(World);
	MessageSystem.BroadcastMessage(LyraGameplayTags::TAG_Lyra_Inventory_Message_SlotChanged, Message);
}

TArray<ULyraInventoryItemInstance*> ITransferableInventory::GetAllItems_Implementation()
{
	return TArray<ULyraInventoryItemInstance*>();
}

void ITransferableInventory::DeleteFromIndex_Implementation(int32 Index)
{
}
