// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abstract/AttributeHandlerComponent.h"
#include "HungerHandlerComponent.generated.h"

class UGTACombatSet;
/**
 * 
 */
UCLASS()
class UKRAINEGTA_API UHungerHandlerComponent : public UAttributeHandlerComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	static UHungerHandlerComponent* FindHungerComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UHungerHandlerComponent>() : nullptr); }
	
	UHungerHandlerComponent(const FObjectInitializer& ObjectInitializer);
	
	virtual void InitializeWithAbilitySystem(ULyraAbilitySystemComponent* InASC) override;
	virtual void UninitializeFromAbilitySystem() override;

	virtual float GetValue() const override;
	virtual float GetMaxValue() const override;

protected:
	virtual void HandleLowValue(AActor* Instigator, AActor* Causer, const FGameplayEffectSpec* EffectSpec, float Magnitude, float OldValue, float NewValue) override;

private:
	// Attribute set used by this component.
	UPROPERTY()
	TObjectPtr<const UGTACombatSet> GTACombatSet;
};
