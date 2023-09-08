// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/LyraAttributeSet.h"

#include "GTACombatSet.generated.h"

class UObject;
struct FFrame;

/**
 * UGTACombatSet
 *
 *  Class that defines attributes that are necessary for applying Armor, Stamina, Hunger
 */
UCLASS(BlueprintType)
class UKRAINEGTA_API UGTACombatSet : public ULyraAttributeSet
{
	GENERATED_BODY()

public:
	UGTACombatSet();


	ATTRIBUTE_ACCESSORS(UGTACombatSet, Armor)
	ATTRIBUTE_ACCESSORS(UGTACombatSet, MaxArmor)
	ATTRIBUTE_ACCESSORS(UGTACombatSet, ArmorPercentReduction)
	ATTRIBUTE_ACCESSORS(UGTACombatSet, ArmorModifier)
	
	ATTRIBUTE_ACCESSORS(UGTACombatSet, Stamina)
	ATTRIBUTE_ACCESSORS(UGTACombatSet, MaxStamina)
	ATTRIBUTE_ACCESSORS(UGTACombatSet, StaminaModifier)
	
	ATTRIBUTE_ACCESSORS(UGTACombatSet, Hunger)
	ATTRIBUTE_ACCESSORS(UGTACombatSet, MaxHunger)
	ATTRIBUTE_ACCESSORS(UGTACombatSet, HungerModifier)
	
	mutable FLyraAttributeEvent OnArmorChanged;
	mutable FLyraAttributeEvent OnMaxArmorChanged;

	mutable FLyraAttributeEvent OnStaminaChanged;
	mutable FLyraAttributeEvent OnMaxStaminaChanged;
	mutable FLyraAttributeEvent OnLowStamina;

	mutable FLyraAttributeEvent OnHungerChanged;
	mutable FLyraAttributeEvent OnMaxHungerChanged;
	mutable FLyraAttributeEvent OnLowHunger;

protected:
	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxArmor(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Stamina(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxStamina(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Hunger(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxHunger(const FGameplayAttributeData& OldValue);
	
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
private:
	bool PreGameplayEffectExecuteArmor(FGameplayEffectModCallbackData& Data);
	bool PreGameplayEffectExecuteStamina(FGameplayEffectModCallbackData& Data);
	bool PreGameplayEffectExecuteHunger(FGameplayEffectModCallbackData& Data);
protected:
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
private:
	void PostGameplayEffectExecuteArmor(const FGameplayEffectModCallbackData& Data);
	void PostGameplayEffectExecuteStamina(const FGameplayEffectModCallbackData& Data);
	void PostGameplayEffectExecuteHunger(const FGameplayEffectModCallbackData& Data);
protected:

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

private:
	UPROPERTY(BlueprintReadOnly, Category = "GTA|Armor", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData ArmorPercentReduction;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Armor, Category = "GTA|Armor", Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Armor;
	UPROPERTY(BlueprintReadOnly, Category="GTA|Armor", Meta=(AllowPrivateAccess=true))
	FGameplayAttributeData ArmorModifier;
	float ArmorBeforeAttributeChange;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxArmor, Category = "GTA|Armor", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxArmor;
	float MaxArmorBeforeAttributeChange;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Stamina, Category = "GTA|Stamina", Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Stamina;
	UPROPERTY(BlueprintReadOnly, Category="GTA|Stamina", Meta=(AllowPrivateAccess=true))
	FGameplayAttributeData StaminaModifier;
	float StaminaBeforeAttributeChange;
	bool bLowStamina;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxStamina, Category = "GTA|Stamina", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxStamina;
	float MaxStaminaBeforeAttributeChange;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Hunger, Category = "GTA|Hunger", Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Hunger;
	UPROPERTY(BlueprintReadOnly, Category="GTA|Hunger", Meta=(AllowPrivateAccess=true))
	FGameplayAttributeData HungerModifier;
	float HungerBeforeAttributeChange;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHunger, Category = "GTA|Hunger", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHunger;
	float MaxHungerBeforeAttributeChange;
};
