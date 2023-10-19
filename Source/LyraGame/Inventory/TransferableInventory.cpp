// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/TransferableInventory.h"

#include "LyraGameplayTags.h"
#include "LyraInventoryItemInstance.h"
#include "GameFramework/GameplayMessageSubsystem.h"

// Add default functionality here for any ITransferableInventory functions that are not pure virtual.
bool ITransferableInventory::TransferSlots_Implementation(UObject* WorldContextObject, FTransferInventoryData Data)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return false;
	}
	
	SetItemAtIndex_Implementation(Data.SourceItem, Data.DestIndex);
	Execute_SetItemAtIndex(Data.SourceInventory, Data.DestItem, Data.SourceIndex);

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(World);
	MessageSystem.BroadcastMessage(LyraGameplayTags::TAG_Lyra_Inventory_Message_SlotSwapped, FTransferInventoryMessage());

	return true;
}

void ITransferableInventory::SetItemAtIndex_Implementation(ULyraInventoryItemInstance* Item, int32 Index)
{
	auto* NewOuter = _getUObject();
	if(Item->GetOuter() != NewOuter)
		Item->Rename(nullptr, NewOuter);
}
