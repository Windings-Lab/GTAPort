// Fill out your copyright notice in the Description page of Project Settings.


#include "Abstract/AttributeHandlerComponent.h"

#include "LyraLogChannels.h"

UAttributeHandlerComponent::UAttributeHandlerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
	
	AbilitySystemComponent = nullptr;
}

void UAttributeHandlerComponent::InitializeWithAbilitySystem(ULyraAbilitySystemComponent* InASC)
{
	AActor* Owner = GetOwner();
	check(Owner);

	if (AbilitySystemComponent)
	{
		UE_LOG(LogLyra, Error, TEXT("AttributeHandlerComponent: Attribute handler component for owner [%s] has already been initialized with an ability system."), *GetNameSafe(Owner));
		return;
	}

	AbilitySystemComponent = InASC;
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogLyra, Error, TEXT("LyraHealthComponent: Cannot initialize attribute handler component for owner [%s] with NULL ability system."), *GetNameSafe(Owner));
		return;
	}
}

void UAttributeHandlerComponent::UninitializeFromAbilitySystem()
{
	AbilitySystemComponent = nullptr;
}

float UAttributeHandlerComponent::GetValue() const
{
	return 0.f;
}

float UAttributeHandlerComponent::GetMaxValue() const
{
	return 0.f;
}

float UAttributeHandlerComponent::GetValueNormalized() const
{
	const float Value = GetValue();
	const float MaxValue = GetMaxValue();
	
	return MaxValue > 0.f ? Value / MaxValue : 0.f;
}

void UAttributeHandlerComponent::OnUnregister()
{
	UninitializeFromAbilitySystem();
	
	Super::OnUnregister();
}

void UAttributeHandlerComponent::HandleValueChanged(AActor* Instigator, AActor* Causer,
	const FGameplayEffectSpec* EffectSpec, float Magnitude, float OldValue, float NewValue)
{
	OnValueChanged.Broadcast(this, OldValue, NewValue, Instigator);
}

void UAttributeHandlerComponent::HandleMaxValueChanged(AActor* Instigator, AActor* Causer,
	const FGameplayEffectSpec* EffectSpec, float Magnitude, float OldValue, float NewValue)
{
	OnMaxValueChanged.Broadcast(this, OldValue, NewValue, Instigator);
}

void UAttributeHandlerComponent::HandleZeroValue(AActor* Instigator, AActor* Causer,
	const FGameplayEffectSpec* EffectSpec, float Magnitude, float OldValue, float NewValue)
{
	OnZeroValue.Broadcast(this, OldValue, NewValue, Instigator);
}

void UAttributeHandlerComponent::HandleLowValue(AActor* Instigator, AActor* Causer,
	const FGameplayEffectSpec* EffectSpec, float Magnitude, float OldValue, float NewValue)
{
	OnLowValue.Broadcast(this, OldValue, NewValue, Instigator);
}
