// Copyright Epic Games, Inc. All Rights Reserved.

#include "LyraInventoryManagerComponent.h"

#include "Engine/ActorChannel.h"
#include "Engine/World.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "LyraInventoryItemDefinition.h"
#include "LyraInventoryItemInstance.h"
#include "NativeGameplayTags.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraInventoryManagerComponent)

class FLifetimeProperty;
struct FReplicationFlags;

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Lyra_Inventory_Message_StackChanged, "Lyra.Inventory.Message.StackChanged");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Lyra_Inventory_Message_SlotSwapped, "Lyra.Inventory.Message.SlotSwapped");

//////////////////////////////////////////////////////////////////////
// FLyraInventoryEntry

FString FLyraInventoryEntry::GetDebugString() const
{
	TSubclassOf<ULyraInventoryItemDefinition> ItemDef;
	int32 StackCount = 0;
	if (Instance && !Instance->IsEmpty())
	{
		ItemDef = Instance->GetItemDef();
		StackCount = Instance->GetStackCount();
	}

	return FString::Printf(TEXT("%s (%d x %s)"), *GetNameSafe(Instance), StackCount, *GetNameSafe(ItemDef));
}

//////////////////////////////////////////////////////////////////////
// FLyraInventoryList

void FLyraInventoryList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (int32 Index : RemovedIndices)
	{
		FLyraInventoryEntry& Entry = Entries[Index];
		if(Entry.Instance->IsStackable())
		{
			BroadcastChangeMessage(Entry, /*OldCount=*/ Entry.Instance->LastStackCount, /*NewCount=*/ 0);
			Entry.Instance->LastStackCount = Entry.Instance->GetStackCount();
		}
		else
		{
			BroadcastChangeMessage(Entry, /*OldCount=*/ 1, /*NewCount=*/ 0);
		}
	}
}

void FLyraInventoryList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		FLyraInventoryEntry& Entry = Entries[Index];
		if(Entry.Instance->IsStackable())
		{
			BroadcastChangeMessage(Entry, /*OldCount=*/ Entry.Instance->LastStackCount, /*NewCount=*/ Entry.Instance->GetStackCount());
			Entry.Instance->LastStackCount = Entry.Instance->GetStackCount();
		}
		else
		{
			BroadcastChangeMessage(Entry, /*OldCount=*/ 0, /*NewCount=*/ 1);
		}
	}
}

void FLyraInventoryList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		FLyraInventoryEntry& Entry = Entries[Index];
		check(Entry.Instance->IsStackable());
		BroadcastChangeMessage(Entry, /*OldCount=*/ Entry.Instance->LastStackCount, /*NewCount=*/ Entry.Instance->GetStackCount());
		Entry.Instance->LastStackCount = Entry.Instance->GetStackCount();
	}
}

void FLyraInventoryList::BroadcastChangeMessage(FLyraInventoryEntry& Entry, int32 OldCount, int32 NewCount)
{
	FLyraInventoryChangeMessage Message;
	Message.InventoryOwner = OwnerComponent;
	Message.Instance = Entry.Instance;
	Message.LastEntryOperation = LastEntryOperation;
	Message.NewCount = NewCount;
	Message.DeltaCount = NewCount - OldCount;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(OwnerComponent->GetWorld());
	MessageSystem.BroadcastMessage(TAG_Lyra_Inventory_Message_StackChanged, Message);
}

void FLyraInventoryList::AddEmptyEntry()
{
	check(OwnerComponent);
	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());
	FLyraInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Instance = NewObject<ULyraInventoryItemInstance>(OwningActor);
	MarkItemDirty(NewEntry);
}

void FLyraInventoryList::AddEmptyEntries(int32 Size)
{
	for(int i = 0; i < Size; i++)
	{
		AddEmptyEntry();
	}
}

ULyraInventoryItemInstance* FLyraInventoryList::ChangeEntry(TSubclassOf<ULyraInventoryItemDefinition> ItemDef, int32 StackCount, EEntryOperation& Op)
{
	check(ItemDef != nullptr)
	ULyraInventoryItemInstance* Result = nullptr;

	TTuple<int32, int32> Index = FindDefAndEmptyIndex(ItemDef);
	const bool bStackable = ItemDef.GetDefaultObject()->bStackable;
	const int32 EntryIndex = bStackable ? Index.Get<0>() : Index.Get<1>();
	
	if(EntryIndex < 0)
	{
		LastEntryOperation = None;
	}
	else
	{
		FLyraInventoryEntry& Entry = Entries[EntryIndex];
		TArray Indices = {EntryIndex};
		
		if(Entry.Instance->IsEmpty())
		{
			if (StackCount <= 0)
			{
				LastEntryOperation = None;
			}
			else
			{
				check(OwnerComponent);
				AActor* OwningActor = OwnerComponent->GetOwner();
				check(OwningActor->HasAuthority());
				Entry.Instance = NewObject<ULyraInventoryItemInstance>(OwningActor);  //@TODO: Using the actor instead of component as the outer due to UE-127172
				Entry.Instance->SetItemDef(ItemDef);
				for (ULyraInventoryItemFragment* Fragment : GetDefault<ULyraInventoryItemDefinition>(ItemDef)->Fragments)
				{
					if (!Fragment) continue;
					Fragment->OnInstanceCreated(Entry.Instance);
				} // End of loop

				if(bStackable)
				{
					Entry.Instance->ItemCount += StackCount;
				}
			
				LastEntryOperation = Added;
				Result = Entry.Instance;

				PostReplicatedAdd(Indices, 0);
			}
		}
		// If it's not empty slot
		else if(Entry.Instance->GetItemDef() == ItemDef)
		{
			if(bStackable)
			{
				Entry.Instance->ItemCount += StackCount;
				// Remove
				if(Entry.Instance->GetStackCount() <= 0)
				{
					LastEntryOperation = Removed;
					PreReplicatedRemove(Indices, 0);
					Result = Entry.Instance;
					Entry.Instance = nullptr;
				}
				// Stack count changed
				else
				{
					LastEntryOperation = Changed;
					PostReplicatedChange(Indices, 0);
				}
			}
			// Remove
			else if(StackCount < 0)
			{
				LastEntryOperation = Removed;
				PreReplicatedRemove(Indices, 0);
				Result = Entry.Instance;
				Entry.Instance = nullptr;
			}
			// TODO: Add as much stack count as possible to empty slots
			else if(StackCount > 0)
			{
			}
		}
		
		MarkItemDirty(Entry);
	}

	Op = LastEntryOperation;
	return Result;
}

TTuple<int32, int32> FLyraInventoryList::FindDefAndEmptyIndex(TSubclassOf<ULyraInventoryItemDefinition> ItemDef)
{
	int32 Index = -1;
	int32 EmptyEntryIndex = -1;
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

	return MakeTuple(Index, EmptyEntryIndex);
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
	EEntryOperation Op = None;
	ULyraInventoryItemInstance* Result = InventoryList.ChangeEntry(ItemDef, StackCount, Op);
	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && Result)
	{
		switch (Op)
		{
		case None:
			break;
		case Changed:
			break;
		case Added:
			AddReplicatedSubObject(Result);
			break;
		case Removed:
			RemoveReplicatedSubObject(Result);
			break;
		default: ;
		}
	}
	return Result;
}

void ULyraInventoryManagerComponent::TransferSlots(int32 SourceIndex, int32 DestIndex)
{
	if(SourceIndex == DestIndex) return;
	InventoryList.Entries.Swap(SourceIndex, DestIndex);

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
	MessageSystem.BroadcastMessage(TAG_Lyra_Inventory_Message_SlotSwapped, FLyraInventoryChangeMessage());
}

void ULyraInventoryManagerComponent::AddEmptySlots(int32 Size)
{
	InventoryList.AddEmptyEntries(Size);
}

TArray<ULyraInventoryItemInstance*> ULyraInventoryManagerComponent::GetAllItems() const
{
	return InventoryList.GetAllItems();
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


