// Copyright Epic Games, Inc. All Rights Reserved.

#include "LyraInventoryManagerComponent.h"

#include "Engine/ActorChannel.h"
#include "Engine/World.h"
#include "LyraInventoryItemDefinition.h"
#include "LyraInventoryItemInstance.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraInventoryManagerComponent)

class FLifetimeProperty;
struct FReplicationFlags;

//////////////////////////////////////////////////////////////////////
// FLyraInventoryEntry

FString FLyraInventoryEntry::GetDebugString() const
{
	TSubclassOf<ULyraInventoryItemDefinition> ItemDef;
	int32 StackCount = 0;
	if (Instance && !Instance->IsEmpty())
	{
		ItemDef = Instance->GetItemDef();
		StackCount = Instance->GetItemCount();
	}

	return FString::Printf(TEXT("%s (%d x %s)"), *GetNameSafe(Instance), StackCount, *GetNameSafe(ItemDef));
}

//////////////////////////////////////////////////////////////////////
// FLyraInventoryList

void FLyraInventoryList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{

}

void FLyraInventoryList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{

}

void FLyraInventoryList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{

}

void FLyraInventoryList::AddEmptyEntry()
{
	check(OwnerComponent);
	check(OwnerComponent->GetOwner()->HasAuthority());
	FLyraInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Instance = NewObject<ULyraInventoryItemInstance>(OwnerComponent);
	MarkItemDirty(NewEntry);
}

void FLyraInventoryList::AddEmptyEntries(int32 Size)
{
	for(int i = 0; i < Size; i++)
	{
		AddEmptyEntry();
	}
}

ULyraInventoryItemInstance* FLyraInventoryList::ChangeEntry(TSubclassOf<ULyraInventoryItemDefinition> ItemDef, int32 StackCount)
{
	if(StackCount == 0) return nullptr;
	check(ItemDef != nullptr)
	
	const int32 Index = FindIndex(ItemDef, StackCount);
	if(Index < 0)
	{
		return nullptr;
	}
	
	FLyraInventoryEntry& Entry = Entries[Index];
	const auto Instance = Entry.Instance;
	auto Result = Entry.Instance;
	TArray Indices = {Index};
	
	if(Instance->IsEmpty())
	{
		if(StackCount <= 0)
		{
			Result = nullptr;
		}
		else
		{
			Instance->CreateNewData(ItemDef, StackCount);
			Result = Instance;
		}
	}
	else if(Instance->GetItemDef() == ItemDef)
	{
		int32 NewItemCount = Instance->GetItemCount() + StackCount;
		Instance->SetItemCount(NewItemCount);
		if(Instance->IsStackable())
		{
			check(NewItemCount >= 0);
			if(NewItemCount == 0)
			{
				Instance->DestroyData();
			}
		}
		else
		{
			// TODO: Implement multiple deletion/insertion
			if(StackCount < 0)
			{
				check(NewItemCount >= 0);
				Instance->DestroyData();
			}
			if(StackCount > 1)
			{
				
			}
		}
	}

	if(Result) MarkItemDirty(Entry);
	
	return Result;
}

void FLyraInventoryList::DeleteFromIndex(int32 Index)
{
	FLyraInventoryEntry& Entry = Entries[Index];
	const auto Instance = Entry.Instance;
	TArray Indices = {Index};

	if(Instance->IsEmpty()) return;
	
	const int32 NewItemCount = Instance->GetItemCount() - 1;
	if(NewItemCount <= 0)
	{
		Instance->DestroyData();
	}
	else
	{
		Instance->SetItemCount(NewItemCount);
	}
}

int32 FLyraInventoryList::FindIndex(TSubclassOf<ULyraInventoryItemDefinition> ItemDef, int32 StackCount)
{
	int32 Index = -1;
	int32 EmptyEntryIndex = -1;
	int32 Result = -1;
	bool bInstanceFound = false;
	bool bEmptyEntryFound = false;
	
	for (int i = 0; i < Entries.Num(); i++)
	{
		if(bInstanceFound && bEmptyEntryFound) break;
		
		auto& Entry = Entries[i];
		if(!Entry.Instance->IsEmpty() && !bInstanceFound && Entry.Instance->GetItemDef() == ItemDef)
		{
			Index = i;
			bInstanceFound = true;
		}
		
		if(Entry.Instance->IsEmpty() && !bEmptyEntryFound)
		{
			EmptyEntryIndex = i;
			bEmptyEntryFound = true;
		}
	}

	Index = bInstanceFound ? Index : EmptyEntryIndex;
	
	if(ItemDef.GetDefaultObject()->bStackable)
	{
		Result = Index;
	}
	else
	{
		Result = StackCount > 0 ? EmptyEntryIndex : Index;
	}

	return Result;
}

TArray<ULyraInventoryItemInstance*> FLyraInventoryList::GetAllItems() const
{
	TArray<ULyraInventoryItemInstance*> Results;
	Results.Reserve(Entries.Num());
	for (const FLyraInventoryEntry& Entry : Entries)
	{
		Results.Add(Entry.Instance);
	}
	
	return Results;
}

//////////////////////////////////////////////////////////////////////
// ULyraInventoryManagerComponent

ULyraInventoryManagerComponent::ULyraInventoryManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, InventoryList(this)
{
	SetIsReplicatedByDefault(true);
}

void ULyraInventoryManagerComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, InventoryList);
}

bool ULyraInventoryManagerComponent::CanAddItemDefinition(TSubclassOf<ULyraInventoryItemDefinition> ItemDef, int32 StackCount)
{
	//@TODO: Add support for stack limit / uniqueness checks / etc...
	return true;
}

ULyraInventoryItemInstance* ULyraInventoryManagerComponent::ChangeInventorySlot(TSubclassOf<ULyraInventoryItemDefinition> ItemDef, int32 StackCount)
{
	ULyraInventoryItemInstance* Result = InventoryList.ChangeEntry(ItemDef, StackCount);
	return Result;
}

void ULyraInventoryManagerComponent::TransferSlots_Implementation(FTransferInventoryData Data)
{
	if(Data.SourceIndex == Data.DestIndex && Data.SourceInventory == this) return;
	Server_TransferSlots(Data);
}

void ULyraInventoryManagerComponent::Server_TransferSlots_Implementation(FTransferInventoryData Data)
{
	ITransferableInventory::TransferSlots_Implementation(Data);
}

TArray<ULyraInventoryItemInstance*> ULyraInventoryManagerComponent::GetAllItems_Implementation()
{
	return InventoryList.GetAllItems();
}

void ULyraInventoryManagerComponent::DeleteFromIndex_Implementation(int32 Index)
{
	Server_DeleteFromIndex(Index);
}

void ULyraInventoryManagerComponent::Server_DeleteFromIndex_Implementation(int32 Index)
{
	InventoryList.DeleteFromIndex(Index);
}

void ULyraInventoryManagerComponent::AddEmptySlots(int32 Size)
{
	InventoryList.AddEmptyEntries(Size);
}

ULyraInventoryItemInstance* ULyraInventoryManagerComponent::FindFirstItemStackByDefinition(TSubclassOf<ULyraInventoryItemDefinition> ItemDef) const
{
	for (const FLyraInventoryEntry& Entry : InventoryList.Entries)
	{
		ULyraInventoryItemInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			if (Instance->GetItemDef() == ItemDef)
			{
				return Instance;
			}
		}
	}

	return nullptr;
}

void ULyraInventoryManagerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	// Register existing ULyraInventoryItemInstance
	if (IsUsingRegisteredSubObjectList())
	{
		for (const FLyraInventoryEntry& Entry : InventoryList.Entries)
		{
			ULyraInventoryItemInstance* Instance = Entry.Instance;

			if (IsValid(Instance))
			{
				AddReplicatedSubObject(Instance);
			}
		}
	}
}

bool ULyraInventoryManagerComponent::ReplicateSubobjects(UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FLyraInventoryEntry& Entry : InventoryList.Entries)
	{
		ULyraInventoryItemInstance* Instance = Entry.Instance;

		if (Instance && IsValid(Instance))
		{
			WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

//////////////////////////////////////////////////////////////////////
//

// UCLASS(Abstract)
// class ULyraInventoryFilter : public UObject
// {
// public:
// 	virtual bool PassesFilter(ULyraInventoryItemInstance* Instance) const { return true; }
// };

// UCLASS()
// class ULyraInventoryFilter_HasTag : public ULyraInventoryFilter
// {
// public:
// 	virtual bool PassesFilter(ULyraInventoryItemInstance* Instance) const { return true; }
// };


