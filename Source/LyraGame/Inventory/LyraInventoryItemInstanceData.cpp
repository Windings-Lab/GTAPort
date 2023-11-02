// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/LyraInventoryItemInstanceData.h"

#include "LyraInventoryItemInstance.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Net/UnrealNetwork.h"
#include "LyraGameplayTags.h"

ULyraInventoryItemInstanceData::ULyraInventoryItemInstanceData()
{
	SetIsReplicatedByDefault(true);
}

void ULyraInventoryItemInstanceData::OnRep_ItemCount()
{
	if(GetOwningActor()->GetLocalRole() == ROLE_AutonomousProxy && ItemCount > 0)
	{
		ULyraInventoryItemInstance* Instance = GetTypedOuter<ULyraInventoryItemInstance>();
		FSlotChangedMessage Message;
		Message.Item = Instance;
		Message.DeltaCount = ItemCount - LastItemCount;
		LastItemCount = ItemCount;
		UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
		MessageSystem.BroadcastMessage(LyraGameplayTags::TAG_Lyra_Inventory_Message_SlotChanged, Message);
	}
}

void ULyraInventoryItemInstanceData::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, StatTags);
	DOREPLIFETIME(ThisClass, ItemDef);
	DOREPLIFETIME(ThisClass, ItemCount);
}

AActor* ULyraInventoryItemInstanceData::GetOwningActor() const
{
	return GetTypedOuter<UActorComponent>()->GetOwner();
}
