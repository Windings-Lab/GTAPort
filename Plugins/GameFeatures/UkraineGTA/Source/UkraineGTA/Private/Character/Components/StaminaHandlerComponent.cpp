// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Components/StaminaHandlerComponent.h"

#include "GTAGameplayTags.h"
#include "LyraLogChannels.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/StaminaAttributeSet.h"

UStaminaHandlerComponent::UStaminaHandlerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
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

void UStaminaHandlerComponent::AfterASCInit()
{
	AActor* Owner = GetOwner();

	AttributeSet = AbilitySystemComponent->GetSet<UStaminaAttributeSet>();
	if (!AttributeSet)
	{
		UE_LOG(LogLyra, Error, TEXT("StaminaHandlerComponent: Cannot initialize stamina handler component for owner [%s] with NULL GTACombatSet on the ability system."), *GetNameSafe(Owner));
	}

	AbilitySystemComponent->SetNumericAttributeBase(UStaminaAttributeSet::GetStaminaAttribute(), AttributeSet->GetMaxValue());
}
