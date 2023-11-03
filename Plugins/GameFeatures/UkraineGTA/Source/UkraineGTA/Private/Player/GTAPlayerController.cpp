// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GTAPlayerController.h"

#include "Equipment/LyraQuickBarComponent.h"
#include "GameModes/AsyncAction_ExperienceReady.h"
#include "Inventory/LyraInventoryManagerComponent.h"

AGTAPlayerController::AGTAPlayerController()
{
	Inventory = CreateDefaultSubobject<ULyraInventoryManagerComponent>(TEXT("Inventory"));
	QuickBar = CreateDefaultSubobject<ULyraQuickBarComponent>(TEXT("QuickBar"));
}

void AGTAPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if(!HasAuthority()) return;

	auto* ActionExperienceReady = UAsyncAction_ExperienceReady::WaitForExperienceReady(GetWorld());
	ActionExperienceReady->OnReady.AddUniqueDynamic(this, &ThisClass::InitializeInventory);
	ActionExperienceReady->Activate();
}

void AGTAPlayerController::InitializeInventory()
{
	Inventory->AddEmptySlots(14);
	QuickBar->AddEmptySlots();
	for(int slotIndex = 0; slotIndex < InitialInventoryItems.Num(); slotIndex++)
	{
		Inventory->ChangeInventorySlot(InitialInventoryItems[slotIndex], 1);
	}

	QuickBar->SetActiveSlotIndex(0);
}
