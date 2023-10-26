// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Attributes/HungerAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "LyraGameplayTags.h"
#include "Net/UnrealNetwork.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_GTA_HungerModified_Message, "GTA.HungerModified.Message");

UHungerAttributeSet::UHungerAttributeSet()
{
}

bool UHungerAttributeSet::CheckLowValue()
{
	if (GetValue() <= 10.f && !bLowValue)
	{
		bLowValue = true;
		return true;
	}
	
	if(GetValue() > 10.f && bLowValue)
	{
		bLowValue = false;
		return false;
	}

	return Super::CheckLowValue();
}

void UHungerAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Hunger, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxHunger, COND_None, REPNOTIFY_Always);
}

bool UHungerAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}
	
	if (Data.EvaluatedData.Magnitude < 0.0f)
	{
#if !UE_BUILD_SHIPPING
		// Check GodMode cheat
		if (Data.Target.HasMatchingGameplayTag(LyraGameplayTags::Cheat_GodMode))
		{
			Data.EvaluatedData.Magnitude = 0.0f;
			return false;
		}
#endif // #if !UE_BUILD_SHIPPING
	}

	return true;
}

void UHungerAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	MinValue = 0.0f;

#if !UE_BUILD_SHIPPING
	if (Data.Target.HasMatchingGameplayTag(LyraGameplayTags::Cheat_GodMode))
	{
		MinValue = 11.0f;
	}
#endif // #if !UE_BUILD_SHIPPING

	MessageTag = TAG_GTA_HungerModified_Message;
	
	Super::PostGameplayEffectExecute(Data);
}
