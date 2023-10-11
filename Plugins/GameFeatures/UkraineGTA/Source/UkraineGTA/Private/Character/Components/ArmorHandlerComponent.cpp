// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Components/ArmorHandlerComponent.h"

#include "LyraLogChannels.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/GTACombatSet.h"

UArmorHandlerComponent::UArmorHandlerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GTACombatSet = nullptr;
}

void UArmorHandlerComponent::InitializeWithAbilitySystem(ULyraAbilitySystemComponent* InASC)
{
	Super::InitializeWithAbilitySystem(InASC);

	AActor* Owner = GetOwner();

	GTACombatSet = AbilitySystemComponent->GetSet<UGTACombatSet>();
	if (!GTACombatSet)
	{
		UE_LOG(LogLyra, Error, TEXT("ArmorHandlerComponent: Cannot initialize armor handler component for owner [%s] with NULL GTACombatSet on the ability system."), *GetNameSafe(Owner));
		return;
	}

	// Register to listen for attribute changes.
	GTACombatSet->OnArmorChanged.AddUObject(this, &ThisClass::HandleValueChanged);
	GTACombatSet->OnMaxArmorChanged.AddUObject(this, &ThisClass::HandleMaxValueChanged);

	OnValueChanged.Broadcast(this, GTACombatSet->GetArmor(), GTACombatSet->GetArmor(), nullptr);
	OnMaxValueChanged.Broadcast(this, GTACombatSet->GetMaxArmor(), GTACombatSet->GetMaxArmor(), nullptr);
}

void UArmorHandlerComponent::UninitializeFromAbilitySystem()
{
	if (GTACombatSet)
	{
		GTACombatSet->OnArmorChanged.RemoveAll(this);
		GTACombatSet->OnMaxArmorChanged.RemoveAll(this);
	}
	
	Super::UninitializeFromAbilitySystem();
	GTACombatSet = nullptr;
}

float UArmorHandlerComponent::GetValue() const
{
	return GTACombatSet ? GTACombatSet->GetArmor() : Super::GetValue();
}

float UArmorHandlerComponent::GetMaxValue() const
{
	return GTACombatSet ? GTACombatSet->GetMaxArmor() : Super::GetMaxValue();
}
