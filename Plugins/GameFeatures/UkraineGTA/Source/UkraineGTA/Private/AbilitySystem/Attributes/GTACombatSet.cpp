// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Attributes/GTACombatSet.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GTACombatSet)

UGTACombatSet::UGTACombatSet() : ArmorPercentReduction(20.f)
{
}

void UGTACombatSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UGTACombatSet, ArmorPercentReduction, COND_OwnerOnly, REPNOTIFY_Always);
}

void UGTACombatSet::OnRep_ArmorBaseReduction(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGTACombatSet, ArmorPercentReduction, OldValue);
}
