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
	SetIsReplicatedByDefault(true);
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

void ULyraInventoryItemInstance::SetItemCount_Implementation(int32 Value)
{
	if(!Data) return;
	Data->ItemCount = Value;
}

bool ULyraInventoryItemInstance::IsStackable() const
{
	return IsEmpty() ? false : Data->ItemDef.GetDefaultObject()->bStackable;
}

void ULyraInventoryItemInstance::CreateNewData_Implementation(TSubclassOf<ULyraInventoryItemDefinition> InDef,
	int32 StackCount)
{
	Data = NewObject<ULyraInventoryItemInstanceData>(this);
	SetItemDef(InDef);
	SetItemCount(StackCount);
}

void ULyraInventoryItemInstance::PreDestroyData_Implementation()
{
	Data->ItemCount = 0;
	FSlotChangedMessage Message;
	Message.Index = Index;
	Message.Item = this;
	Message.DeltaCount = 0 - Data->LastItemCount;
	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
	MessageSystem.BroadcastMessage(LyraGameplayTags::TAG_Lyra_Inventory_Message_SlotChanged, Message);
}

void ULyraInventoryItemInstance::DestroyData_Implementation()
{
	PreDestroyData();
	Data->DestroyComponent();
	Data = nullptr;
}

void ULyraInventoryItemInstance::Swap_Implementation(ULyraInventoryItemInstance* Other)
{
	const auto TempData = Data;
	Data = Other->Data;
	
	Other->Data = TempData;
}

void ULyraInventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Data);
}

#if UE_WITH_IRIS
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

void ULyraInventoryItemInstance::SetItemDef_Implementation(TSubclassOf<ULyraInventoryItemDefinition> InDef)
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

void ULyraInventoryItemInstance::OnRep_Data()
{
	if(!Data) return;
	
	FSlotChangedMessage Message;
	Message.Index = Index;
	Message.Item = this;
	Data->LastItemCount = Data->ItemCount;
	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
	MessageSystem.BroadcastMessage(LyraGameplayTags::TAG_Lyra_Inventory_Message_SlotChanged, Message);
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
