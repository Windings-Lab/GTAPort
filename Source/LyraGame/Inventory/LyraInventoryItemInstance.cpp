// Copyright Epic Games, Inc. All Rights Reserved.

#include "LyraInventoryItemInstance.h"

#include "Inventory/LyraInventoryItemDefinition.h"
#include "Net/UnrealNetwork.h"

#if UE_WITH_IRIS
#include "Iris/ReplicationSystem/ReplicationFragmentUtil.h"
#endif // UE_WITH_IRIS

#include "LyraGameplayTags.h"
#include "LyraInventoryItemInstanceData.h"
#include "GameFramework/GameplayMessageSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraInventoryItemInstance)

class FLifetimeProperty;

ULyraInventoryItemInstance::ULyraInventoryItemInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool ULyraInventoryItemInstance::IsEmpty() const
{
	return Data == nullptr;
}

int32 ULyraInventoryItemInstance::GetItemCount() const
{
	if(!Data) return 0;
	return Data->ItemCount;
}

void ULyraInventoryItemInstance::SetItemCount(int32 Value)
{
	if(!Data) return;
	Data->ItemCount = Value;
}

bool ULyraInventoryItemInstance::IsStackable() const
{
	return IsEmpty() ? false : Data->ItemDef.GetDefaultObject()->bStackable;
}

void ULyraInventoryItemInstance::CreateNewData(TSubclassOf<ULyraInventoryItemDefinition> InDef, int32 StackCount)
{
	Data = NewObject<ULyraInventoryItemInstanceData>(GetOuter());
	SetItemDef(InDef);
	SetItemCount(StackCount);
	BroadcastAddMessage();
}

void ULyraInventoryItemInstance::DestroyData()
{
	BroadcastDeleteMessage();
	Data = nullptr;
}

void ULyraInventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Data);
}

#if UE_WITH_IRIS
void ULyraInventoryItemInstance::Swap(ULyraInventoryItemInstance* Other)
{
	const auto TempData = Data;
	Data = Other->Data;
	Other->Data = TempData;
}

void ULyraInventoryItemInstance::BroadcastAddMessage()
{
	FSlotChangedMessage Message;
	Message.Index = Index;
	Message.Item = this;
	Message.DeltaCount = Data->ItemCount;
	Data->LastItemCount = Data->ItemCount;
	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
	MessageSystem.BroadcastMessage(LyraGameplayTags::TAG_Lyra_Inventory_Message_SlotChanged, Message);
}

void ULyraInventoryItemInstance::BroadcastDeleteMessage()
{
	FSlotChangedMessage Message;
	Message.Index = Index;
	Message.Item = this;
	Message.DeltaCount = Data->ItemCount - Data->LastItemCount;
	Data->LastItemCount = Data->ItemCount;
	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
	MessageSystem.BroadcastMessage(LyraGameplayTags::TAG_Lyra_Inventory_Message_SlotChanged, Message);
}

void ULyraInventoryItemInstance::BroadcastChangeMessage()
{
	FSlotChangedMessage Message;
	Message.Index = Index;
	Message.Item = this;
	Message.DeltaCount = Data->LastItemCount - Data->ItemCount;
	Message.Item->Data->LastItemCount = Data->ItemCount;
	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
	MessageSystem.BroadcastMessage(LyraGameplayTags::TAG_Lyra_Inventory_Message_SlotChanged, Message);
}

void ULyraInventoryItemInstance::RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags)
{
	using namespace UE::Net;

	// Build descriptors and allocate PropertyReplicationFragments for this object
	FReplicationFragmentUtil::CreateAndRegisterFragmentsForObject(this, Context, RegistrationFlags);
}
#endif // UE_WITH_IRIS

void ULyraInventoryItemInstance::AddStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	if(!Data) return;
	Data->StatTags.AddStack(Tag, StackCount);
}

void ULyraInventoryItemInstance::RemoveStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	if(!Data) return;
	Data->StatTags.RemoveStack(Tag, StackCount);
}

int32 ULyraInventoryItemInstance::GetStatTagStackCount(FGameplayTag Tag) const
{
	return Data ? Data->StatTags.GetStackCount(Tag) : false;
}

bool ULyraInventoryItemInstance::HasStatTag(FGameplayTag Tag) const
{
	return Data ? Data->StatTags.ContainsTag(Tag) : false;
}

TSubclassOf<ULyraInventoryItemDefinition> ULyraInventoryItemInstance::GetItemDef() const
{
	return Data ? Data->ItemDef : nullptr;
}

void ULyraInventoryItemInstance::SetItemDef(TSubclassOf<ULyraInventoryItemDefinition> InDef)
{
	check(Data != nullptr);
	Data->ItemDef = InDef;

	if(Data->ItemDef)
	{
		for (const auto Fragment : GetDefault<ULyraInventoryItemDefinition>(Data->ItemDef)->Fragments)
		{
			if (!Fragment) continue;
			Fragment->OnInstanceCreated(this);
		}
	}
}

const ULyraInventoryItemFragment* ULyraInventoryItemInstance::FindFragmentByClassConst(TSubclassOf<ULyraInventoryItemFragment> FragmentClass) const
{
	if(!Data) return nullptr;
	
	if ((Data->ItemDef != nullptr) && (FragmentClass != nullptr))
	{
		return GetDefault<ULyraInventoryItemDefinition>(Data->ItemDef)->FindFragmentByClass(FragmentClass);
	}

	return nullptr;
}

ULyraInventoryItemFragment* ULyraInventoryItemInstance::FindFragmentByClass(
	TSubclassOf<ULyraInventoryItemFragment> FragmentClass)
{
	return const_cast<ULyraInventoryItemFragment*>(FindFragmentByClassConst(FragmentClass));
}
