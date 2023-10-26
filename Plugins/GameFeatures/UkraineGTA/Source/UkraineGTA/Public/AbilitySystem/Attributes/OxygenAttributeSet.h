// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "Abstract/AbilitySystem/Attributes/GTABaseAttributeSet.h"
#include "OxygenAttributeSet.generated.h"

UKRAINEGTA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GTA_OxygenModified_Message);

/**
 * 
 */
UCLASS()
class UKRAINEGTA_API UOxygenAttributeSet : public UGTABaseAttributeSet
{
	GENERATED_BODY()

public:
	UOxygenAttributeSet();

	GTA_ATTRIBUTES(Oxygen)

protected:
	virtual bool CheckLowValue() override;

	OVERRIDE_GTA_SET_ACCESSORS(Oxygen)

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

private:
	UFUNCTION()
	void OnRep_Value(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass,Oxygen, OldValue) }
	UFUNCTION()
	void OnRep_MaxValue(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxOxygen, OldValue) }
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Value, Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Oxygen;
	UPROPERTY(BlueprintReadOnly, Meta=(AllowPrivateAccess=true))
	FGameplayAttributeData Modifier_Oxygen;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxValue, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxOxygen;
};
