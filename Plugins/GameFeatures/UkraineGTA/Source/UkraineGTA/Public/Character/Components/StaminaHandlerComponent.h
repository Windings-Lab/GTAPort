// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Abstract/AttributeHandlerComponent.h"
#include "StaminaHandlerComponent.generated.h"

class UGTACombatSet;
/**
 * 
 */
UCLASS()
class UKRAINEGTA_API UStaminaHandlerComponent : public UAttributeHandlerComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	static UStaminaHandlerComponent* FindStaminaComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UStaminaHandlerComponent>() : nullptr); }
	
	UStaminaHandlerComponent(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void HandleLowValue(AActor* Instigator, AActor* Causer, const FGameplayEffectSpec* EffectSpec, float Magnitude, float OldValue, float NewValue) override;
	virtual void AfterASCInit() override;
};
