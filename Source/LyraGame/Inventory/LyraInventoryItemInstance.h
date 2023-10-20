// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "LyraInventoryItemInstance.generated.h"

class ULyraInventoryItemInstanceData;
class ULyraInventoryItemInstance;
class FLifetimeProperty;

class ULyraInventoryItemDefinition;
class ULyraInventoryItemFragment;
struct FFrame;
struct FGameplayTag;

USTRUCT(BlueprintType)
struct FSlotChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int32 Index = -1;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<ULyraInventoryItemInstance> Item = nullptr;
	UPROPERTY(BlueprintReadWrite)
	int32 DeltaCount = 0;
};

/**
 * ULyraInventoryItemInstance
 */
UCLASS(BlueprintType)
class ULyraInventoryItemInstance : public UObject
{
	GENERATED_BODY()

public:
	ULyraInventoryItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category=Inventory)
	bool IsEmpty() const;

	UFUNCTION(BlueprintCallable, Category=Inventory)
	int32 GetItemCount() const;

	void SetItemCount(int32 Value);

	UFUNCTION(BlueprintCallable, Category=Inventory)
	bool IsStackable() const;

	void CreateNewData(TSubclassOf<ULyraInventoryItemDefinition> InDef, int32 StackCount);
	void DestroyData();
	
	//~UObject interface
	virtual bool IsSupportedForNetworking() const override { return true; }
	//~End of UObject interface

	// Adds a specified number of stacks to the tag (does nothing if StackCount is below 1)
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	void AddStatTagStack(FGameplayTag Tag, int32 StackCount);

	// Removes a specified number of stacks from the tag (does nothing if StackCount is below 1)
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category= Inventory)
	void RemoveStatTagStack(FGameplayTag Tag, int32 StackCount);

	// Returns the stack count of the specified tag (or 0 if the tag is not present)
	UFUNCTION(BlueprintCallable, Category=Inventory)
	int32 GetStatTagStackCount(FGameplayTag Tag) const;

	// Returns true if there is at least one stack of the specified tag
	UFUNCTION(BlueprintCallable, Category=Inventory)
	bool HasStatTag(FGameplayTag Tag) const;

	UFUNCTION(BlueprintCallable, Category=Inventory)
	TSubclassOf<ULyraInventoryItemDefinition> GetItemDef() const;

	UFUNCTION(BlueprintCallable, BlueprintPure=false, meta=(DeterminesOutputType=FragmentClass))
	const ULyraInventoryItemFragment* FindFragmentByClassConst(TSubclassOf<ULyraInventoryItemFragment> FragmentClass) const;

	template <typename ResultClass>
	const ResultClass* FindFragmentByClass() const
	{
		return (ResultClass*)this->FindFragmentByClass(ResultClass::StaticClass());
	}

	UFUNCTION(BlueprintCallable, BlueprintPure=false, meta=(DeterminesOutputType=FragmentClass))
	ULyraInventoryItemFragment* FindFragmentByClass(TSubclassOf<ULyraInventoryItemFragment> FragmentClass);

	template <typename ResultClass>
	ResultClass* FindFragmentByClass()
	{
		return (ResultClass*)this->FindFragmentByClass(ResultClass::StaticClass());
	}

	void Swap(ULyraInventoryItemInstance* Other);

	void BroadcastAddMessage();
	void BroadcastDeleteMessage();
	void BroadcastChangeMessage();

private:
#if UE_WITH_IRIS
	/** Register all replication fragments */
	virtual void RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags) override;
#endif // UE_WITH_IRIS

	void SetItemDef(TSubclassOf<ULyraInventoryItemDefinition> InDef);
	
private:
	UPROPERTY(Replicated)
	TObjectPtr<ULyraInventoryItemInstanceData> Data = nullptr;
	
	UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess))
	int32 Index = -1;

	UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess))
	TObjectPtr<UUserWidget> AttachedWidget = nullptr;
};
