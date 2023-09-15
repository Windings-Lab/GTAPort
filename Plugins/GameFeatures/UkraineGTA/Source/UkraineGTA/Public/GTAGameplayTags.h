// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"

namespace GTAGameplayTags
{
	UKRAINEGTA_API FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);

	UKRAINEGTA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Debuff_LowHunger);
	UKRAINEGTA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Debuff_LowStamina);
}