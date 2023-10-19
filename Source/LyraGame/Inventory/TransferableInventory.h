// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TransferableInventory.generated.h"

class ULyraInventoryItemInstance;

USTRUCT(BlueprintType)
struct FTransferInventoryData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int32 SourceIndex;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<ULyraInventoryItemInstance> SourceItem;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UObject> SourceInventory;

	UPROPERTY(BlueprintReadWrite)
	int32 DestIndex;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<ULyraInventoryItemInstance> DestItem;
};

USTRUCT(BlueprintType)
struct FTransferInventoryMessage
{
	GENERATED_BODY()
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
	bool TransferSlots(UObject* WorldContextObject, FTransferInventoryData Data);
	virtual bool TransferSlots_Implementation(UObject* WorldContextObject, FTransferInventoryData Data);

	UFUNCTION(BlueprintNativeEvent)
	void SetItemAtIndex(ULyraInventoryItemInstance* Item, int32 Index);
	virtual void SetItemAtIndex_Implementation(ULyraInventoryItemInstance* Item, int32 Index);
};
