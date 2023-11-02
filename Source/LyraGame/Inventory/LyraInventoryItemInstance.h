// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "LyraInventoryItemInstance.generated.h"

class ULyraInventoryTileWidget;
class UInventoryTileWidget;
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
	TObjectPtr<ULyraInventoryItemInstance> Item = nullptr;
	UPROPERTY(BlueprintReadWrite)
	int32 DeltaCount = 0;
};

/**
 * ULyraInventoryItemInstance
 */
UCLASS(BlueprintType)
class LYRAGAME_API ULyraInventoryItemInstance : public UActorComponent
{
	GENERATED_BODY()

public:
	ULyraInventoryItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category=Inventory)
	bool IsEmpty() const;

	UFUNCTION(BlueprintCallable, Category=Inventory)
	int32 GetItemCount() const;

	UFUNCTION(Server, Reliable)
	void SetItemCount(int32 Value);

	UFUNCTION(BlueprintCallable, Category=Inventory)
	bool IsStackable() const;

	UFUNCTION(Server, Reliable)
	void CreateNewData(TSubclassOf<ULyraInventoryItemDefinition> InDef, int32 StackCount);
	UFUNCTION(Server, Reliable)
	void DestroyData();
	UFUNCTION(Client, Reliable)
	void PreDestroyData();

	void SetWidget(ULyraInventoryTileWidget* InWidget);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SpawnPickUpItem(TSubclassOf<AActor> Class);

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

	UFUNCTION(Server, Reliable)
	void Swap(ULyraInventoryItemInstance* Other);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	

private:
#if UE_WITH_IRIS
	/** Register all replication fragments */
	virtual void RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags) override;
#endif // UE_WITH_IRIS

	UFUNCTION(Server, Reliable)
	void SetItemDef(TSubclassOf<ULyraInventoryItemDefinition> InDef);

	UFUNCTION()
	void OnRep_Data();
	
private:
	friend ULyraInventoryItemInstanceData;
	
	UPROPERTY(ReplicatedUsing=OnRep_Data)
	TObjectPtr<ULyraInventoryItemInstanceData> Data = nullptr;
	
	UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess))
	int32 Index = -1;

	UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess))
	TObjectPtr<ULyraInventoryTileWidget> AttachedWidget = nullptr;
};
