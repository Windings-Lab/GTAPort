// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/LyraAttributeSet.h"
#include "GTABaseAttributeSet.generated.h"

#define GTA_ATTRIBUTES(PropertyName) \
	ATTRIBUTE_ACCESSORS(ThisClass, PropertyName) \
	ATTRIBUTE_ACCESSORS(ThisClass, Modifier_##PropertyName) \
	ATTRIBUTE_ACCESSORS(ThisClass, Max##PropertyName)

#define OVERRIDE_GTA_SET_ACCESSORS(PropertyName) \
	OVERRIDE_SET_FUNCTIONS(Value, PropertyName) \
	OVERRIDE_SET_FUNCTIONS(Modifier_Value, Modifier_##PropertyName) \
	OVERRIDE_SET_FUNCTIONS(MaxValue, Max##PropertyName)

#define OVERRIDE_SET_FUNCTIONS(PropertyName, ChildPropertyName) \
	OVERRIDE_ATTRIBUTE_PROPERTY_GETTER(PropertyName, ChildPropertyName) \
	OVERRIDE_ATTRIBUTE_VALUE_GETTER(PropertyName, ChildPropertyName) \
	OVERRIDE_ATTRIBUTE_VALUE_SETTER(PropertyName, ChildPropertyName)

#define OVERRIDE_ATTRIBUTE_PROPERTY_GETTER(PropertyName, ChildPropertyName) \
	virtual const FGameplayAttribute Get##PropertyName##Attribute() const override \
	{ \
		return Get##ChildPropertyName##Attribute(); \
	}

#define OVERRIDE_ATTRIBUTE_VALUE_GETTER(PropertyName, ChildPropertyName) \
	virtual float Get##PropertyName() const override \
	{ \
		return Get##ChildPropertyName(); \
	}

#define OVERRIDE_ATTRIBUTE_VALUE_SETTER(PropertyName, ChildPropertyName) \
	virtual void Set##PropertyName(float NewVal) override \
	{ \
		Set##ChildPropertyName(NewVal); \
	}

#define VIRTUAL_SET_FUNCTIONS(PropertyName) \
	VIRTUAL_ATTRIBUTE_PROPERTY_GETTER(PropertyName) \
	VIRTUAL_ATTRIBUTE_VALUE_GETTER(PropertyName) \
	VIRTUAL_ATTRIBUTE_VALUE_SETTER(PropertyName)

#define VIRTUAL_ATTRIBUTE_PROPERTY_GETTER(PropertyName) \
	virtual const FGameplayAttribute Get##PropertyName##Attribute() const PURE_VIRTUAL(ThisClass::Get##PropertyName##Attribute, return nullptr;);

#define VIRTUAL_ATTRIBUTE_VALUE_GETTER(PropertyName) \
	virtual float Get##PropertyName() const PURE_VIRTUAL(ThisClass::Get##PropertyName, return 0.f;);

#define VIRTUAL_ATTRIBUTE_VALUE_SETTER(PropertyName) \
	virtual void Set##PropertyName(float NewVal) PURE_VIRTUAL(ThisClass::Set##PropertyName,); \

struct FGameplayTag;
/**
 * 
 */
UCLASS(Abstract, BlueprintType)
class UKRAINEGTA_API UGTABaseAttributeSet : public ULyraAttributeSet
{
	GENERATED_BODY()

public:
	UGTABaseAttributeSet();

	mutable FLyraAttributeEvent OnValueChanged;
	mutable FLyraAttributeEvent OnMaxValueChanged;
	mutable FLyraAttributeEvent OnLowValue;

	VIRTUAL_SET_FUNCTIONS(Value)
	VIRTUAL_SET_FUNCTIONS(Modifier_Value)
	VIRTUAL_SET_FUNCTIONS(MaxValue)
	#undef VIRTUAL_SET_FUNCTIONS

protected:
	virtual bool CheckLowValue();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

private:
	void SendMessage(const FGameplayEffectModCallbackData& Data, const FGameplayTag& GameplayTag);

protected:
	FGameplayTag MessageTag;
	
	float BeforeAttributeChange_Value;
	float MinValue;
	bool bLowValue;
	
	float BeforeAttributeChange_MaxValue;
};
