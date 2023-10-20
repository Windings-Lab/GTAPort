// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "System/GameplayTagStack.h"
#include "LyraInventoryItemInstanceData.generated.h"

class ULyraInventoryItemDefinition;
/**
 * 
 */
UCLASS()
class LYRAGAME_API ULyraInventoryItemInstanceData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(Replicated)
	FGameplayTagStackContainer StatTags;
	
	UPROPERTY(Replicated)
	TSubclassOf<ULyraInventoryItemDefinition> ItemDef;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	int32 ItemCount;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	int32 LastItemCount;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
