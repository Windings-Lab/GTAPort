// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/LyraPlayerController.h"
#include "GTAPlayerController.generated.h"

class ULyraQuickBarComponent;
class ULyraInventoryManagerComponent;
class ULyraInventoryItemDefinition;
/**
 * 
 */
UCLASS()
class UKRAINEGTA_API AGTAPlayerController : public ALyraPlayerController
{
	GENERATED_BODY()

public:
	AGTAPlayerController();
	
protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void InitializeInventory();

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory", Meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<ULyraInventoryItemDefinition>> InitialInventoryItems;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ULyraInventoryManagerComponent> Inventory;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ULyraQuickBarComponent> QuickBar;
};
