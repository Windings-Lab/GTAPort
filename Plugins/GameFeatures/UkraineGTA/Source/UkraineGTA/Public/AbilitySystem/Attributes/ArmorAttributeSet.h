// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystemComponent.h"
#include "NativeGameplayTags.h"
#include "Abstract/AbilitySystem/Attributes/GTABaseAttributeSet.h"
#include "ArmorAttributeSet.generated.h"

UKRAINEGTA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GTA_ArmorModified_Message);

/**
 * 
 */
UCLASS(BlueprintType)
class UKRAINEGTA_API UArmorAttributeSet : public UGTABaseAttributeSet
{
	GENERATED_BODY()

public:
	UArmorAttributeSet();

	ATTRIBUTE_ACCESSORS(ThisClass, ArmorPercentReduction)
	ATTRIBUTE_ACCESSORS(ThisClass, Armor)
	ATTRIBUTE_ACCESSORS(ThisClass, Modifier_Armor)
	ATTRIBUTE_ACCESSORS(ThisClass, MaxArmor)

protected:
	OVERRIDE_GTA_SET_ACCESSORS(Armor)

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;


private:
	UFUNCTION()
	void OnRep_Value(const FGameplayAttributeData& OldValue)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Armor, OldValue)
	}

	UFUNCTION()
	void OnRep_MaxValue(const FGameplayAttributeData& OldValue)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxArmor, OldValue)
	}

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Value, Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Armor;
	UPROPERTY(BlueprintReadOnly, Meta=(AllowPrivateAccess=true))
	FGameplayAttributeData Modifier_Armor;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxValue, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxArmor;
	
	UPROPERTY(BlueprintReadOnly, Category = "GTA|Armor", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData ArmorPercentReduction;
};
