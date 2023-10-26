// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Components/OxygenHandlerComponent.h"

#include "GTAGameplayTags.h"
#include "LyraLogChannels.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/OxygenAttributeSet.h"

UOxygenHandlerComponent::UOxygenHandlerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UOxygenHandlerComponent::HandleLowValue(AActor* Instigator, AActor* Causer, const FGameplayEffectSpec* EffectSpec,
	float Magnitude, float OldValue, float NewValue)
{
	FGameplayEventData Payload;

	Payload.EventTag = GTAGameplayTags::Ability_Debuff_LowOxygen;
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

void UOxygenHandlerComponent::AfterASCInit()
{
	AActor* Owner = GetOwner();

	AttributeSet = AbilitySystemComponent->GetSet<UOxygenAttributeSet>();
	if (!AttributeSet)
	{
		UE_LOG(LogLyra, Error, TEXT("OxygenHandlerComponent: Cannot initialize oxygen handler component for owner [%s] with NULL OxygenAttributeSet on the ability system."), *GetNameSafe(Owner));
	}
}
