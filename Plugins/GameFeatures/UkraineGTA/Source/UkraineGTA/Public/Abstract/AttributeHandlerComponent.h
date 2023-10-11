// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameFrameworkComponent.h"
#include "AttributeHandlerComponent.generated.h"

class ULyraAttributeSet;
struct FGameplayEffectSpec;
class ULyraAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FValue_AttributeChanged
	, UAttributeHandlerComponent*, AttributeHandlerComponent
	, float, OldValue
	, float, NewValue
	, AActor*, Instigator);
/**
 * 
 */
UCLASS(Abstract)
class UKRAINEGTA_API UAttributeHandlerComponent : public UGameFrameworkComponent
{
	GENERATED_BODY()

public:
	UAttributeHandlerComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Attribute")
	virtual void InitializeWithAbilitySystem(ULyraAbilitySystemComponent* InASC);

	UFUNCTION(BlueprintCallable, Category = "Attribute")
	virtual void UninitializeFromAbilitySystem();

	UFUNCTION(BlueprintCallable, Category = "Attribute")
	virtual float GetValue() const;

	UFUNCTION(BlueprintCallable, Category = "Attribute")
	virtual float GetMaxValue() const;

	UFUNCTION(BlueprintCallable, Category = "Attribute")
	float GetValueNormalized() const;

public:
	UPROPERTY(BlueprintAssignable)
	FValue_AttributeChanged OnValueChanged;
	
	UPROPERTY(BlueprintAssignable)
	FValue_AttributeChanged OnMaxValueChanged;

	UPROPERTY(BlueprintAssignable)
	FValue_AttributeChanged OnZeroValue;
	
	UPROPERTY(BlueprintAssignable)
	FValue_AttributeChanged OnLowValue;

protected:
	virtual void OnUnregister() override;
	
	void HandleValueChanged(AActor* Instigator, AActor* Causer, const FGameplayEffectSpec* EffectSpec, float Magnitude, float OldValue, float NewValue);
	void HandleMaxValueChanged(AActor* Instigator, AActor* Causer, const FGameplayEffectSpec* EffectSpec, float Magnitude, float OldValue, float NewValue);
	
	virtual void HandleZeroValue(AActor* Instigator, AActor* Causer, const FGameplayEffectSpec* EffectSpec, float Magnitude, float OldValue, float NewValue);
	virtual void HandleLowValue(AActor* Instigator, AActor* Causer, const FGameplayEffectSpec* EffectSpec, float Magnitude, float OldValue, float NewValue);

protected:
	// Ability system used by this component.
	UPROPERTY()
	TObjectPtr<ULyraAbilitySystemComponent> AbilitySystemComponent;
};
