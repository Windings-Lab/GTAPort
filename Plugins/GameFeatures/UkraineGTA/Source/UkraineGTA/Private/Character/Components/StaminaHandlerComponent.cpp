// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Components/StaminaHandlerComponent.h"

#include "GTAGameplayTags.h"
#include "LyraLogChannels.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/GTACombatSet.h"

UStaminaHandlerComponent::UStaminaHandlerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GTACombatSet = nullptr;
}

void UStaminaHandlerComponent::InitializeWithAbilitySystem(ULyraAbilitySystemComponent* InASC)
{
	Super::InitializeWithAbilitySystem(InASC);

	AActor* Owner = GetOwner();

	GTACombatSet = AbilitySystemComponent->GetSet<UGTACombatSet>();
	if (!GTACombatSet)
	{
		UE_LOG(LogLyra, Error, TEXT("StaminaHandlerComponent: Cannot initialize stamina handler component for owner [%s] with NULL GTACombatSet on the ability system."), *GetNameSafe(Owner));
		return;
	}

	// Register to listen for attribute changes.
	GTACombatSet->OnStaminaChanged.AddUObject(this, &ThisClass::HandleValueChanged);
	GTACombatSet->OnMaxStaminaChanged.AddUObject(this, &ThisClass::HandleMaxValueChanged);
	GTACombatSet->OnLowStamina.AddUObject(this, &ThisClass::HandleLowValue);

	OnValueChanged.Broadcast(this, GTACombatSet->GetStamina(), GTACombatSet->GetStamina(), nullptr);
	OnMaxValueChanged.Broadcast(this, GTACombatSet->GetMaxStamina(), GTACombatSet->GetMaxStamina(), nullptr);
}

void UStaminaHandlerComponent::UninitializeFromAbilitySystem()
{
	if (GTACombatSet)
	{
		GTACombatSet->OnStaminaChanged.RemoveAll(this);
		GTACombatSet->OnMaxStaminaChanged.RemoveAll(this);
		GTACombatSet->OnLowStamina.RemoveAll(this);
	}
	
	Super::UninitializeFromAbilitySystem();
	GTACombatSet = nullptr;
}

float UStaminaHandlerComponent::GetValue() const
{
	return GTACombatSet ? GTACombatSet->GetStamina() : Super::GetValue();
}

float UStaminaHandlerComponent::GetMaxValue() const
{
	return GTACombatSet ? GTACombatSet->GetMaxStamina() : Super::GetMaxValue();
}

void UStaminaHandlerComponent::HandleLowValue(AActor* Instigator, AActor* Causer, const FGameplayEffectSpec* EffectSpec,
	float Magnitude, float OldValue, float NewValue)
{
	FGameplayEventData Payload;

	Payload.EventTag = GTAGameplayTags::Ability_Debuff_LowStamina;
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
