// Copyright Epic Games, Inc. All Rights Reserved.

#include "LyraQuickBarComponent.h"

#include "LyraGameplayTags.h"
#include "Equipment/LyraEquipmentDefinition.h"
#include "Equipment/LyraEquipmentInstance.h"
#include "Equipment/LyraEquipmentManagerComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/Pawn.h"
#include "Inventory/InventoryFragment_EquippableItem.h"
#include "NativeGameplayTags.h"
#include "Inventory/LyraInventoryManagerComponent.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraQuickBarComponent)

class FLifetimeProperty;
class ULyraEquipmentDefinition;

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Lyra_QuickBar_Message_SlotsChanged, "Lyra.QuickBar.Message.SlotsChanged");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Lyra_QuickBar_Message_ActiveIndexChanged, "Lyra.QuickBar.Message.ActiveIndexChanged");

ULyraQuickBarComponent::ULyraQuickBarComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

bool ULyraQuickBarComponent::TransferSlots_Implementation(UObject* WorldContextObject, FTransferInventoryData Data)
{
	if(Data.SourceIndex == Data.DestIndex && Data.SourceInventory == this) return false;
	return ITransferableInventory::TransferSlots_Implementation(WorldContextObject, Data);
}

void ULyraQuickBarComponent::SetItemAtIndex_Implementation(ULyraInventoryItemInstance* Item, int32 Index)
{
	ITransferableInventory::SetItemAtIndex_Implementation(Item, Index);
	bool bNewItem = Slots[0]->GetItemDef() != Item->GetItemDef() && Index == 0;
	Slots[Index] = Item;
	if(!Slots[0]->IsEmpty())
	{
		if(bNewItem)
		{
			ActiveSlotIndex = -1;
		}
		SetActiveSlotIndex(0);
	}
	else
	{
		UnequipItemInSlot();
		ActiveSlotIndex = -1;
	}
}

void ULyraQuickBarComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Slots);
	DOREPLIFETIME(ThisClass, ActiveSlotIndex);
}

void ULyraQuickBarComponent::BeginPlay()
{
	for(int i = 0; i < NumSlots; i++)
	{
		auto& Slot = Slots.AddDefaulted_GetRef();
		Slot = NewObject<ULyraInventoryItemInstance>(this);
	}

	Super::BeginPlay();
}

void ULyraQuickBarComponent::CycleActiveSlotForward()
{
	if (Slots.Num() < 2)
	{
		return;
	}

	const int32 OldIndex = (ActiveSlotIndex < 0 ? Slots.Num()-1 : ActiveSlotIndex);
	int32 NewIndex = ActiveSlotIndex;
	do
	{
		NewIndex = (NewIndex + 1) % Slots.Num();
		if (Slots[NewIndex] != nullptr)
		{
			SetActiveSlotIndex(NewIndex);
			return;
		}
	} while (NewIndex != OldIndex);
}

void ULyraQuickBarComponent::CycleActiveSlotBackward()
{
	if (Slots.Num() < 2)
	{
		return;
	}

	const int32 OldIndex = (ActiveSlotIndex < 0 ? Slots.Num()-1 : ActiveSlotIndex);
	int32 NewIndex = ActiveSlotIndex;
	do
	{
		NewIndex = (NewIndex - 1 + Slots.Num()) % Slots.Num();
		if (Slots[NewIndex] != nullptr)
		{
			SetActiveSlotIndex(NewIndex);
			return;
		}
	} while (NewIndex != OldIndex);
}

void ULyraQuickBarComponent::EquipItemInSlot()
{
	check(Slots.IsValidIndex(ActiveSlotIndex));
	check(EquippedItem == nullptr);

	if (ULyraInventoryItemInstance* SlotItem = Slots[ActiveSlotIndex])
	{
		if (const UInventoryFragment_EquippableItem* EquipInfo = SlotItem->FindFragmentByClass<UInventoryFragment_EquippableItem>())
		{
			TSubclassOf<ULyraEquipmentDefinition> EquipDef = EquipInfo->EquipmentDefinition;
			if (EquipDef != nullptr)
			{
				if (ULyraEquipmentManagerComponent* EquipmentManager = FindEquipmentManager())
				{
					EquippedItem = EquipmentManager->EquipItem(EquipDef);
					if (EquippedItem != nullptr)
					{
						EquippedItem->SetInstigator(SlotItem);
					}
				}
			}
		}
	}
}

void ULyraQuickBarComponent::UnequipItemInSlot()
{
	if (ULyraEquipmentManagerComponent* EquipmentManager = FindEquipmentManager())
	{
		if (EquippedItem != nullptr)
		{
			EquipmentManager->UnequipItem(EquippedItem);
			EquippedItem = nullptr;
		}
	}
}

ULyraEquipmentManagerComponent* ULyraQuickBarComponent::FindEquipmentManager() const
{
	if (AController* OwnerController = Cast<AController>(GetOwner()))
	{
		if (APawn* Pawn = OwnerController->GetPawn())
		{
			return Pawn->FindComponentByClass<ULyraEquipmentManagerComponent>();
		}
	}
	return nullptr;
}

void ULyraQuickBarComponent::SetActiveSlotIndex_Implementation(int32 NewIndex)
{
	if (Slots.IsValidIndex(NewIndex) && (ActiveSlotIndex != NewIndex))
	{
		UnequipItemInSlot();

		ActiveSlotIndex = NewIndex;

		EquipItemInSlot();

		OnRep_ActiveSlotIndex();
	}
}

ULyraInventoryItemInstance* ULyraQuickBarComponent::GetActiveSlotItem() const
{
	return Slots.IsValidIndex(ActiveSlotIndex) ? Slots[ActiveSlotIndex] : nullptr;
}

int32 ULyraQuickBarComponent::GetNextFreeItemSlot() const
{
	int32 SlotIndex = 0;
	for (TObjectPtr<ULyraInventoryItemInstance> ItemPtr : Slots)
	{
		if (ItemPtr == nullptr)
		{
			return SlotIndex;
		}
		++SlotIndex;
	}

	return INDEX_NONE;
}

void ULyraQuickBarComponent::AddItemToSlot(int32 SlotIndex, ULyraInventoryItemInstance* Item)
{
	if (Slots.IsValidIndex(SlotIndex) && (Item != nullptr))
	{
		if (Slots[SlotIndex] == nullptr)
		{
			Slots[SlotIndex] = Item;
			OnRep_Slots();
		}
	}
}

ULyraInventoryItemInstance* ULyraQuickBarComponent::RemoveItemFromSlot(int32 SlotIndex)
{
	ULyraInventoryItemInstance* Result = nullptr;

	if (ActiveSlotIndex == SlotIndex)
	{
		UnequipItemInSlot();
		ActiveSlotIndex = -1;
	}

	if (Slots.IsValidIndex(SlotIndex))
	{
		Result = Slots[SlotIndex];

		if (Result != nullptr)
		{
			Slots[SlotIndex] = nullptr;
			OnRep_Slots();
		}
	}

	return Result;
}

void ULyraQuickBarComponent::OnRep_Slots()
{
	FLyraQuickBarSlotsChangedMessage Message;
	Message.Owner = GetOwner();
	Message.Slots = Slots;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	MessageSystem.BroadcastMessage(TAG_Lyra_QuickBar_Message_SlotsChanged, Message);
}

void ULyraQuickBarComponent::OnRep_ActiveSlotIndex()
{
	FLyraQuickBarActiveIndexChangedMessage Message;
	Message.Owner = GetOwner();
	Message.ActiveIndex = ActiveSlotIndex;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	MessageSystem.BroadcastMessage(TAG_Lyra_QuickBar_Message_ActiveIndexChanged, Message);
}

