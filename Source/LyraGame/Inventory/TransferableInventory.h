// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TransferableInventory.generated.h"

struct FSlotChangedMessage;
class ULyraInventoryItemInstance;

USTRUCT(BlueprintType)
struct FTransferInventoryData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int32 SourceIndex = -1;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<ULyraInventoryItemInstance> SourceItem = nullptr;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UObject> SourceInventory = nullptr;

	UPROPERTY(BlueprintReadWrite)
	int32 DestIndex = -1;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<ULyraInventoryItemInstance> DestItem = nullptr;
};


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTransferableInventory : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LYRAGAME_API ITransferableInventory
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void TransferSlots(UObject* WorldContextObject, FTransferInventoryData Data);
	virtual void TransferSlots_Implementation(UObject* WorldContextObject, FTransferInventoryData Data);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	TArray<ULyraInventoryItemInstance*> GetAllItems();
	virtual TArray<ULyraInventoryItemInstance*> GetAllItems_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void DeleteFromIndex(int32 Index);
};
