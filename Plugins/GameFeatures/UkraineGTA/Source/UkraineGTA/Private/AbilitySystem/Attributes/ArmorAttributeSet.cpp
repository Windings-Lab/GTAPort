// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Attributes/ArmorAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "LyraGameplayTags.h"
#include "Net/UnrealNetwork.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_GTA_ArmorModified_Message, "GTA.ArmorModified.Message");

UArmorAttributeSet::UArmorAttributeSet()
{
}

void UArmorAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxArmor, COND_None, REPNOTIFY_Always);
}

bool UArmorAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}

	if (Data.EvaluatedData.Magnitude < 0.0f)
	{
		const bool bIsDamageFromSelfDestruct = Data.EffectSpec.GetDynamicAssetTags().HasTagExact(LyraGameplayTags::TAG_Gameplay_DamageSelfDestruct);

		if (Data.Target.HasMatchingGameplayTag(LyraGameplayTags::TAG_Gameplay_DamageImmunity) && !bIsDamageFromSelfDestruct)
		{
			Data.EvaluatedData.Magnitude = 0.0f;
			return false;
		}

#if !UE_BUILD_SHIPPING
		// Check GodMode cheat
		if (Data.Target.HasMatchingGameplayTag(LyraGameplayTags::Cheat_GodMode) && !bIsDamageFromSelfDestruct)
		{
			Data.EvaluatedData.Magnitude = 0.0f;
			return false;
		}
#endif // #if !UE_BUILD_SHIPPING
	}

	return true;
}

void UArmorAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	const bool bIsDamageFromSelfDestruct = Data.EffectSpec.GetDynamicAssetTags().HasTagExact(LyraGameplayTags::TAG_Gameplay_DamageSelfDestruct);
	MinValue = 0.0f;

#if !UE_BUILD_SHIPPING
	if (!bIsDamageFromSelfDestruct && Data.Target.HasMatchingGameplayTag(LyraGameplayTags::Cheat_GodMode))
	{
		MinValue = 1.0f;
	}
#endif // #if !UE_BUILD_SHIPPING

	MessageTag = TAG_GTA_ArmorModified_Message;
	
	Super::PostGameplayEffectExecute(Data);
}
