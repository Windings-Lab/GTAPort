// Fill out your copyright notice in the Description page of Project Settings.

#include "GTAGameplayTags.h"

namespace GTAGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Debuff_LowHunger, "Ability.Debuff.LowHunger", "Low hunger debuff decreasing HP of character");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Debuff_LowStamina, "Ability.Debuff.LowStamina", "Low stamina removes ability to jump/run");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Debuff_LowOxygen, "Ability.Debuff.LowOxygen", "Low stamina removes ability to jump/run");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Behavior_Water_Swimming, "Ability.Behavior.Water.Swimming", "Tag added when swimming in water");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Behavior_Water_AffectedByWater, "Ability.Behavior.Water.AffectedByWater", "Water movement disabling abilities to jump, sprint, crouch");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Behavior_Water_UnderWater, "Ability.Behavior.Water.UnderWater", "Oxygen drain and can swim up");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Native_Swim, "InputTag.Native.Swim", "Character Swimming Input");
}