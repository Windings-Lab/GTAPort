// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"
#include "Abstract/AbilitySystem/Attributes/GTABaseAttributeSet.h"
#include "HungerAttributeSet.generated.h"

UKRAINEGTA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GTA_HungerModified_Message);

/**
 * 
 */
UCLASS()
class UKRAINEGTA_API UHungerAttributeSet : public UGTABaseAttributeSet
{
	GENERATED_BODY()

public:
	UHungerAttributeSet();

	GTA_ATTRIBUTES(Hunger)

protected:
	virtual bool CheckLowValue() override;

	OVERRIDE_GTA_SET_ACCESSORS(Hunger)
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

private:
	UFUNCTION()
	void OnRep_Value(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass,Hunger, OldValue) }
	UFUNCTION()
	void OnRep_MaxValue(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxHunger, OldValue) }
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Value, Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Hunger;
	UPROPERTY(BlueprintReadOnly, Meta=(AllowPrivateAccess=true))
	FGameplayAttributeData Modifier_Hunger;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxValue, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHunger;
};
