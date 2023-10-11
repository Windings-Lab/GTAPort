// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Components/HungerHandlerComponent.h"

#include "GTAGameplayTags.h"
#include "LyraLogChannels.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/GTACombatSet.h"

UHungerHandlerComponent::UHungerHandlerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GTACombatSet = nullptr;
}

void UHungerHandlerComponent::InitializeWithAbilitySystem(ULyraAbilitySystemComponent* InASC)
{
	Super::InitializeWithAbilitySystem(InASC);

	AActor* Owner = GetOwner();

	GTACombatSet = AbilitySystemComponent->GetSet<UGTACombatSet>();
	if (!GTACombatSet)
	{
		UE_LOG(LogLyra, Error, TEXT("HungerHandlerComponent: Cannot initialize hunger handler component for owner [%s] with NULL GTACombatSet on the ability system."), *GetNameSafe(Owner));
		return;
	}

	// Register to listen for attribute changes.
	GTACombatSet->OnHungerChanged.AddUObject(this, &ThisClass::HandleValueChanged);
	GTACombatSet->OnMaxHungerChanged.AddUObject(this, &ThisClass::HandleMaxValueChanged);
	GTACombatSet->OnLowHunger.AddUObject(this, &ThisClass::HandleLowValue);

	OnValueChanged.Broadcast(this, GTACombatSet->GetHunger(), GTACombatSet->GetHunger(), nullptr);
	OnMaxValueChanged.Broadcast(this, GTACombatSet->GetMaxHunger(), GTACombatSet->GetMaxHunger(), nullptr);
}

void UHungerHandlerComponent::UninitializeFromAbilitySystem()
{
	if (GTACombatSet)
	{
		GTACombatSet->OnHungerChanged.RemoveAll(this);
		GTACombatSet->OnMaxHungerChanged.RemoveAll(this);
		GTACombatSet->OnLowHunger.RemoveAll(this);
	}
	
	Super::UninitializeFromAbilitySystem();
	GTACombatSet = nullptr;
}

float UHungerHandlerComponent::GetValue() const
{
	return GTACombatSet ? GTACombatSet->GetHunger() : Super::GetValue();
}

float UHungerHandlerComponent::GetMaxValue() const
{
	return GTACombatSet ? GTACombatSet->GetMaxHunger() : Super::GetMaxValue();
}

void UHungerHandlerComponent::HandleLowValue(AActor* Instigator, AActor* Causer, const FGameplayEffectSpec* EffectSpec,
	float Magnitude, float OldValue, float NewValue)
{
	FGameplayEventData Payload;
	Payload.EventTag = GTAGameplayTags::Ability_Debuff_LowHunger;
	Payload.Instigator = Instigator;
	Payload.Target = Causer;
	Payload.OptionalObject = EffectSpec->Def;
	Payload.ContextHandle = EffectSpec->GetEffectContext();
	Payload.InstigatorTags = *EffectSpec->CapturedSourceTags.GetAggregatedTags();
	Payload.TargetTags = *EffectSpec->CapturedTargetTags.GetAggregatedTags();
	Payload.EventMagnitude = Magnitude;

	FScopedPredictionWindow NewScopedWindow(AbilitySystemComponent, true);
	AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);
	
	Super::HandleLowValue(Instigator, Causer, EffectSpec, Magnitude, OldValue, NewValue);
}
