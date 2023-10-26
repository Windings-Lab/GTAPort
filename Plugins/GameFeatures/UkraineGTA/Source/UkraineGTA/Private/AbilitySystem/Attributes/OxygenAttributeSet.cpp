// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Attributes/OxygenAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "LyraGameplayTags.h"
#include "Net/UnrealNetwork.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_GTA_OxygenModified_Message, "GTA.OxygenModified.Message");

UOxygenAttributeSet::UOxygenAttributeSet()
{
}

bool UOxygenAttributeSet::CheckLowValue()
{
	if (GetValue() <= 0.f && !bLowValue)
	{
		bLowValue = true;
		return true;
	}
	
	if(GetValue() > 0.f && bLowValue)
	{
		bLowValue = false;
		return false;
	}

	return Super::CheckLowValue();
}

void UOxygenAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Oxygen, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxOxygen, COND_None, REPNOTIFY_Always);
}

bool UOxygenAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
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

void UOxygenAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	MinValue = 0.0f;

#if !UE_BUILD_SHIPPING
	if (Data.Target.HasMatchingGameplayTag(LyraGameplayTags::Cheat_GodMode))
	{
		MinValue = 1.0f;
	}
#endif // #if !UE_BUILD_SHIPPING

	MessageTag = TAG_GTA_OxygenModified_Message;
	
	Super::PostGameplayEffectExecute(Data);
}
