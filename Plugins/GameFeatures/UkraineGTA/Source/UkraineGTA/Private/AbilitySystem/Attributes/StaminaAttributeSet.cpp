// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Attributes/StaminaAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "LyraGameplayTags.h"
#include "Net/UnrealNetwork.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_GTA_StaminaModified_Message, "GTA.StaminaModified.Message");

UStaminaAttributeSet::UStaminaAttributeSet()
{
}

bool UStaminaAttributeSet::CheckLowValue()
{
	if (GetValue() <= 20.f && !bLowValue)
	{
		bLowValue = true;
		return true;
	}
	
	if(GetValue() > 20.f && bLowValue)
	{
		bLowValue = false;
		return false;
	}

	return Super::CheckLowValue();
}

void UStaminaAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxStamina, COND_None, REPNOTIFY_Always);
}

bool UStaminaAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
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

void UStaminaAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	MinValue = 0.0f;

#if !UE_BUILD_SHIPPING
	if (Data.Target.HasMatchingGameplayTag(LyraGameplayTags::Cheat_GodMode))
	{
		MinValue = 21.0f;
	}
#endif // #if !UE_BUILD_SHIPPING

	MessageTag = TAG_GTA_StaminaModified_Message;
	
	Super::PostGameplayEffectExecute(Data);
}
