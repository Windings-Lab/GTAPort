// Fill out your copyright notice in the Description page of Project Settings.


#include "Abstract/AbilitySystem/Attributes/GTABaseAttributeSet.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Messages/LyraVerbMessage.h"

UGTABaseAttributeSet::UGTABaseAttributeSet()
	: BeforeAttributeChange_Value(0), MinValue(0), bLowValue(false), BeforeAttributeChange_MaxValue(0)
{
}

bool UGTABaseAttributeSet::CheckLowValue()
{
	return false;
}

void UGTABaseAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UGTABaseAttributeSet::SendMessage(const FGameplayEffectModCallbackData& Data, const FGameplayTag& GameplayTag)
{
	FLyraVerbMessage Message;
	Message.Verb = GameplayTag;
	Message.Instigator = Data.EffectSpec.GetEffectContext().GetEffectCauser();
	Message.InstigatorTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
	Message.Target = GetOwningActor();
	Message.TargetTags = *Data.EffectSpec.CapturedTargetTags.GetAggregatedTags();
	//@TODO: Fill out context tags, and any non-ability-system source/instigator tags
	//@TODO: Determine if it's an opposing team kill, self-own, team kill, etc...
	Message.Magnitude = Data.EvaluatedData.Magnitude;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
	MessageSystem.BroadcastMessage(Message.Verb, Message);
}

bool UGTABaseAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if(!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}
	
	BeforeAttributeChange_Value = GetValue();
	BeforeAttributeChange_MaxValue = GetMaxValue();

	return true;
}

void UGTABaseAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();

	if (Data.EvaluatedData.Attribute == GetModifier_ValueAttribute())
	{
		SendMessage(Data, MessageTag);
		
		SetValue(FMath::Clamp(GetValue() + GetModifier_Value(), MinValue, GetMaxValue()));
		SetModifier_Value(0.f);
	}
	else if (Data.EvaluatedData.Attribute == GetValueAttribute())
	{
		SetValue(FMath::Clamp(GetValue(), MinValue, GetMaxValue()));
	}
	else if (Data.EvaluatedData.Attribute == GetMaxValueAttribute())
	{
		OnMaxValueChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, BeforeAttributeChange_MaxValue, GetMaxValue());
	}
	
	if (GetValue() != BeforeAttributeChange_Value)
	{
		OnValueChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, BeforeAttributeChange_Value, GetValue());
	}

	if(CheckLowValue())
	{
		OnLowValue.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, BeforeAttributeChange_Value, GetValue());
	}
}

void UGTABaseAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	ClampAttribute(Attribute, NewValue);
}

void UGTABaseAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	ClampAttribute(Attribute, NewValue);
}

void UGTABaseAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	ULyraAbilitySystemComponent* LyraASC = GetLyraAbilitySystemComponent();
	check(LyraASC);
	
	if (Attribute == GetMaxValueAttribute())
	{
		if (GetValue() > NewValue) LyraASC->ApplyModToAttribute(GetValueAttribute(), EGameplayModOp::Override, NewValue);
	}
}

void UGTABaseAttributeSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetValueAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxValue());
	}
	else if(Attribute == GetMaxValueAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.0f);
	}
}
