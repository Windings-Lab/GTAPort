// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/ControllerComponent.h"
#include "Inventory/LyraInventoryItemInstance.h"
#include "Inventory/TransferableInventory.h"

#include "LyraQuickBarComponent.generated.h"

class AActor;
class ULyraEquipmentInstance;
class ULyraEquipmentManagerComponent;
class UObject;
struct FFrame;

USTRUCT(BlueprintType)
struct FActiveSlotChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 NewActiveSlotIndex;
};

UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class LYRAGAME_API ULyraQuickBarComponent : public UControllerComponent, public ITransferableInventory
{
	GENERATED_BODY()

public:
	ULyraQuickBarComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category=Inventory)
	bool TransferSlots(UObject* WorldContextObject, FTransferInventoryData Data);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category=Inventory)
	void SetItemAtIndex(ULyraInventoryItemInstance* Item, int32 Index);

	UFUNCTION(BlueprintCallable, Category="Lyra")
	void CycleActiveSlotForward();

	UFUNCTION(BlueprintCallable, Category="Lyra")
	void CycleActiveSlotBackward();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Lyra")
	void SetActiveSlotIndex(int32 NewIndex);

	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	TArray<ULyraInventoryItemInstance*> GetSlots() const
	{
		return Slots;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	int32 GetActiveSlotIndex() const { return ActiveSlotIndex; }

	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	ULyraInventoryItemInstance* GetActiveSlotItem() const;

	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	int32 GetNextFreeItemSlot() const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddItemToSlot(int32 SlotIndex, ULyraInventoryItemInstance* Item);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	ULyraInventoryItemInstance* RemoveItemFromSlot(int32 SlotIndex);

	virtual void BeginPlay() override;

private:
	void UnequipItemInSlot();
	void EquipItemInSlot();

	ULyraEquipmentManagerComponent* FindEquipmentManager() const;

protected:
	UPROPERTY()
	int32 NumSlots = 4;

	UFUNCTION()
	void OnRep_Slots();

	UFUNCTION()
	void OnRep_ActiveSlotIndex();

private:
	UPROPERTY(ReplicatedUsing=OnRep_Slots)
	TArray<TObjectPtr<ULyraInventoryItemInstance>> Slots;

	UPROPERTY(ReplicatedUsing=OnRep_ActiveSlotIndex)
	int32 ActiveSlotIndex = -1;

	UPROPERTY()
	TObjectPtr<ULyraEquipmentInstance> EquippedItem;
};


USTRUCT(BlueprintType)
struct FLyraQuickBarSlotsChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	TObjectPtr<AActor> Owner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TArray<TObjectPtr<ULyraInventoryItemInstance>> Slots;
};


USTRUCT(BlueprintType)
struct FLyraQuickBarActiveIndexChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	TObjectPtr<AActor> Owner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	int32 ActiveIndex = 0;
};
