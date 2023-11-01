// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "System/GameplayTagStack.h"
#include "LyraInventoryItemInstanceData.generated.h"

class ULyraInventoryItemDefinition;
/**
 * 
 */
UCLASS()
class LYRAGAME_API ULyraInventoryItemInstanceData : public UActorComponent
{
	GENERATED_BODY()

public:
	ULyraInventoryItemInstanceData();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UFUNCTION()
	void OnRep_ItemCount();
	AActor* GetOwningActor() const;

public:
	UPROPERTY(Replicated)
	FGameplayTagStackContainer StatTags;
	
	UPROPERTY(Replicated)
	TSubclassOf<ULyraInventoryItemDefinition> ItemDef;

	UPROPERTY(ReplicatedUsing=OnRep_ItemCount, VisibleAnywhere, BlueprintReadOnly)
	int32 ItemCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 LastItemCount;
};
