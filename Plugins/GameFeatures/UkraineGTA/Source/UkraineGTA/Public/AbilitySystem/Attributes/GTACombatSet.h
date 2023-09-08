// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/LyraAttributeSet.h"

#include "GTACombatSet.generated.h"

class UObject;
struct FFrame;

/**
 * 
 */
UCLASS()
class UKRAINEGTA_API UGTACombatSet : public ULyraAttributeSet
{
	GENERATED_BODY()

public:
	UGTACombatSet();

	ATTRIBUTE_ACCESSORS(UGTACombatSet, ArmorPercentReduction)

	UFUNCTION()
	void OnRep_ArmorBaseReduction(const FGameplayAttributeData& OldValue);

private:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ArmorBaseReduction, Category = "GTA|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData ArmorPercentReduction;
};
