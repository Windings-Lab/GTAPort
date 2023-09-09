// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Attributes/GTACombatSet.h"

#include "GameplayEffectExtension.h"
#include "LyraGameplayTags.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/LyraHealthSet.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Messages/LyraVerbMessage.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GTACombatSet)

UGTACombatSet::UGTACombatSet() :
	ArmorPercentReduction(0.f), Armor(0.f), ArmorBeforeAttributeChange(0.f), MaxArmor(250.f), MaxArmorBeforeAttributeChange(0.f)
	, Stamina(100.f), StaminaBeforeAttributeChange(0.f), MaxStamina(100.f), MaxStaminaBeforeAttributeChange(0.f)
	, Hunger(100.f), HungerBeforeAttributeChange(0.f), MaxHunger(100.f), MaxHungerBeforeAttributeChange(0.f)
{
}

void UGTACombatSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UGTACombatSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGTACombatSet, MaxArmor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGTACombatSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGTACombatSet, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGTACombatSet, Hunger, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGTACombatSet, MaxHunger, COND_None, REPNOTIFY_Always);
}

void UGTACombatSet::OnRep_Armor(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGTACombatSet, Armor, OldValue);

	const float CurrentArmor = GetArmor();
	const float EstimatedMagnitude = CurrentArmor - OldValue.GetCurrentValue();
	
	OnArmorChanged.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentArmor);
}

void UGTACombatSet::OnRep_MaxArmor(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGTACombatSet, MaxArmor, OldValue);

	OnMaxArmorChanged.Broadcast(nullptr, nullptr, nullptr, GetMaxArmor() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetMaxArmor());
}

void UGTACombatSet::OnRep_Stamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGTACombatSet, Stamina, OldValue);
}

void UGTACombatSet::OnRep_MaxStamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGTACombatSet, MaxStamina, OldValue);
}

void UGTACombatSet::OnRep_Hunger(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGTACombatSet, Hunger, OldValue);
}

void UGTACombatSet::OnRep_MaxHunger(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGTACombatSet, MaxHunger, OldValue);
}

bool UGTACombatSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}
	
	bool Result = true;
	if (Data.EvaluatedData.Attribute == GetArmorModifierAttribute())
	{
		Result = PreGameplayEffectExecuteArmor(Data);
	}
	else if(Data.EvaluatedData.Attribute == GetStaminaModifierAttribute())
	{
		Result = PreGameplayEffectExecuteStamina(Data);
	}

	return Result;
}

bool UGTACombatSet::PreGameplayEffectExecuteArmor(FGameplayEffectModCallbackData& Data)
{
	if (Data.EvaluatedData.Magnitude < 0.0f)
	{
		const bool bIsDamageFromSelfDestruct = Data.EffectSpec.GetDynamicAssetTags().HasTagExact(TAG_Gameplay_DamageSelfDestruct);

		if (Data.Target.HasMatchingGameplayTag(TAG_Gameplay_DamageImmunity) && !bIsDamageFromSelfDestruct)
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

	ArmorBeforeAttributeChange = GetArmor();
	MaxArmorBeforeAttributeChange = GetMaxArmor();

	return true;
}

bool UGTACombatSet::PreGameplayEffectExecuteStamina(FGameplayEffectModCallbackData& Data)
{
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

	StaminaBeforeAttributeChange = GetStamina();
	MaxStaminaBeforeAttributeChange = GetMaxStamina();

	return true;
}

bool UGTACombatSet::PreGameplayEffectExecuteHunger(FGameplayEffectModCallbackData& Data)
{
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

	HungerBeforeAttributeChange = GetHunger();
	MaxHungerBeforeAttributeChange = GetMaxHunger();

	return true;
}

void UGTACombatSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if(Data.EvaluatedData.Attribute == GetArmorModifierAttribute()
		|| Data.EvaluatedData.Attribute == GetArmorAttribute()
		|| Data.EvaluatedData.Attribute == GetMaxArmorAttribute())
	{
		PostGameplayEffectExecuteArmor(Data);
	}
	else if(Data.EvaluatedData.Attribute == GetStaminaModifierAttribute()
		|| Data.EvaluatedData.Attribute == GetStaminaAttribute()
		|| Data.EvaluatedData.Attribute == GetMaxStaminaAttribute())
	{
		PostGameplayEffectExecuteStamina(Data);
	}
}

void UGTACombatSet::PostGameplayEffectExecuteArmor(const FGameplayEffectModCallbackData& Data)
{
	const bool bIsDamageFromSelfDestruct = Data.EffectSpec.GetDynamicAssetTags().HasTagExact(TAG_Gameplay_DamageSelfDestruct);
	float MinimumArmor = 0.0f;

#if !UE_BUILD_SHIPPING
	if (!bIsDamageFromSelfDestruct && Data.Target.HasMatchingGameplayTag(LyraGameplayTags::Cheat_GodMode))
	{
		MinimumArmor = 1.0f;
	}
#endif // #if !UE_BUILD_SHIPPING

	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();

	if (Data.EvaluatedData.Attribute == GetArmorModifierAttribute())
	{
		// Send a standardized verb message that other systems can observe
		if (Data.EvaluatedData.Magnitude < 0.0f)
		{
			FLyraVerbMessage Message;
			Message.Verb = TAG_Lyra_Damage_Message;
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
		
		SetArmor(FMath::Clamp(GetArmor() + GetArmorModifier(), MinimumArmor, GetMaxArmor()));
		SetArmorModifier(0.f);
	}
	else if (Data.EvaluatedData.Attribute == GetArmorAttribute())
	{
		SetArmor(FMath::Clamp(GetArmor(), MinimumArmor, GetMaxArmor()));
	}
	else if (Data.EvaluatedData.Attribute == GetMaxArmorAttribute())
	{
		OnMaxArmorChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, MaxArmorBeforeAttributeChange, GetMaxArmor());
	}
	
	if (GetArmor() != ArmorBeforeAttributeChange)
	{
		OnArmorChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, ArmorBeforeAttributeChange, GetArmor());
	}
}

void UGTACombatSet::PostGameplayEffectExecuteStamina(const FGameplayEffectModCallbackData& Data)
{
	float MinimumStamina = 0.0f;

#if !UE_BUILD_SHIPPING
	if (Data.Target.HasMatchingGameplayTag(LyraGameplayTags::Cheat_GodMode))
	{
		MinimumStamina = 1.0f;
	}
#endif // #if !UE_BUILD_SHIPPING

	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();

	if (Data.EvaluatedData.Attribute == GetStaminaModifierAttribute())
	{
		// Send a standardized verb message that other systems can observe
		if (Data.EvaluatedData.Magnitude < 0.0f)
		{
			/*FLyraVerbMessage Message;
			Message.Verb = TAG_Lyra_Damage_Message;
			Message.Instigator = Data.EffectSpec.GetEffectContext().GetEffectCauser();
			Message.InstigatorTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
			Message.Target = GetOwningActor();
			Message.TargetTags = *Data.EffectSpec.CapturedTargetTags.GetAggregatedTags();
			Message.Magnitude = Data.EvaluatedData.Magnitude;

			UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
			MessageSystem.BroadcastMessage(Message.Verb, Message);*/
		}
		
		SetStamina(FMath::Clamp(GetStamina() + GetStaminaModifier(), MinimumStamina, GetMaxStamina()));
		SetStaminaModifier(0.f);
	}
	else if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), MinimumStamina, GetMaxStamina()));
	}
	else if (Data.EvaluatedData.Attribute == GetMaxStaminaAttribute())
	{
		OnMaxStaminaChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, MaxStaminaBeforeAttributeChange, GetMaxStamina());
	}
	
	if (GetStamina() != StaminaBeforeAttributeChange)
	{
		OnStaminaChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, StaminaBeforeAttributeChange, GetStamina());
	}

	if (GetStamina() <= 20.f && !bLowStamina)
	{
		bLowStamina = true;
		OnLowStamina.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, StaminaBeforeAttributeChange, GetStamina());
	}
	else if(GetStamina() > 20.f && bLowStamina)
	{
		bLowStamina = false;
	}
}

void UGTACombatSet::PostGameplayEffectExecuteHunger(const FGameplayEffectModCallbackData& Data)
{
		float MinimumHunger = 0.0f;

#if !UE_BUILD_SHIPPING
	if (Data.Target.HasMatchingGameplayTag(LyraGameplayTags::Cheat_GodMode))
	{
		MinimumHunger = 1.0f;
	}
#endif // #if !UE_BUILD_SHIPPING

	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();

	if (Data.EvaluatedData.Attribute == GetHungerModifierAttribute())
	{
		// Send a standardized verb message that other systems can observe
		if (Data.EvaluatedData.Magnitude < 0.0f)
		{
			/*FLyraVerbMessage Message;
			Message.Verb = TAG_Lyra_Damage_Message;
			Message.Instigator = Data.EffectSpec.GetEffectContext().GetEffectCauser();
			Message.InstigatorTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
			Message.Target = GetOwningActor();
			Message.TargetTags = *Data.EffectSpec.CapturedTargetTags.GetAggregatedTags();
			Message.Magnitude = Data.EvaluatedData.Magnitude;

			UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
			MessageSystem.BroadcastMessage(Message.Verb, Message);*/
		}
		
		SetHunger(FMath::Clamp(GetHunger() + GetHungerModifier(), MinimumHunger, GetMaxHunger()));
		SetHungerModifier(0.f);
	}
	else if (Data.EvaluatedData.Attribute == GetHungerAttribute())
	{
		SetHunger(FMath::Clamp(GetHunger(), MinimumHunger, GetMaxHunger()));
	}
	else if (Data.EvaluatedData.Attribute == GetMaxHungerAttribute())
	{
		OnMaxHungerChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, MaxHungerBeforeAttributeChange, GetMaxHunger());
	}
	
	if (GetHunger() != HungerBeforeAttributeChange)
	{
		OnHungerChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, HungerBeforeAttributeChange, GetHunger());
	}

	if (GetHunger() <= 10.f)
	{
		OnLowHunger.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, HungerBeforeAttributeChange, GetHunger());
	}
}

void UGTACombatSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	ClampAttribute(Attribute, NewValue);
}

void UGTACombatSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	ClampAttribute(Attribute, NewValue);
}

void UGTACombatSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	ULyraAbilitySystemComponent* LyraASC = GetLyraAbilitySystemComponent();
	check(LyraASC);

	if (Attribute == GetMaxArmorAttribute())
	{
		if (GetArmor() > NewValue) LyraASC->ApplyModToAttribute(GetArmorAttribute(), EGameplayModOp::Override, NewValue);
	}
	else if (Attribute == GetMaxStaminaAttribute())
	{
		if (GetStamina() > NewValue) LyraASC->ApplyModToAttribute(GetStaminaAttribute(), EGameplayModOp::Override, NewValue);
	}
	else if (Attribute == GetMaxHungerAttribute())
	{
		if (GetHunger() > NewValue) LyraASC->ApplyModToAttribute(GetHungerAttribute(), EGameplayModOp::Override, NewValue);
	}
}

void UGTACombatSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetArmorAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxArmor());
	}
	else if(Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxStamina());
	}
	else if(Attribute == GetHungerAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHunger());
	}
	else if (Attribute == GetMaxArmorAttribute() || Attribute == GetMaxStaminaAttribute() || Attribute == GetMaxHungerAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.0f);
	}
}
