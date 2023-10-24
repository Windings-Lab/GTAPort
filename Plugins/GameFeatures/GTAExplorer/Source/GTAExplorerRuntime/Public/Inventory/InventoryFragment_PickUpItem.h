// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/LyraInventoryItemDefinition.h"
#include "InventoryFragment_PickUpItem.generated.h"

/**
 * 
 */
UCLASS()
class GTAEXPLORERRUNTIME_API UInventoryFragment_PickUpItem : public ULyraInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Appearance)
	TSubclassOf<AActor> DroppedItemToSpawn;
};
