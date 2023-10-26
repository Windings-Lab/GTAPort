// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"
#include "Abstract/AbilitySystem/Attributes/GTABaseAttributeSet.h"
#include "StaminaAttributeSet.generated.h"

UKRAINEGTA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GTA_StaminaModified_Message);

/**
 * 
 */
UCLASS()
class UKRAINEGTA_API UStaminaAttributeSet : public UGTABaseAttributeSet
{
	GENERATED_BODY()

public:
	UStaminaAttributeSet();
	
	GTA_ATTRIBUTES(Stamina)

protected:
	virtual bool CheckLowValue() override;

	OVERRIDE_GTA_SET_ACCESSORS(Stamina)

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

private:
	UFUNCTION() void OnRep_Value(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass,Stamina, OldValue) }
	UFUNCTION() void OnRep_MaxValue(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxStamina, OldValue) }
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Value, Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Stamina;
	UPROPERTY(BlueprintReadOnly, Meta=(AllowPrivateAccess=true))
	FGameplayAttributeData Modifier_Stamina;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxValue, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxStamina;
};
