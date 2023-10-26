// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abstract/AttributeHandlerComponent.h"
#include "OxygenHandlerComponent.generated.h"

/**
 * 
 */
UCLASS()
class UKRAINEGTA_API UOxygenHandlerComponent : public UAttributeHandlerComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	static UOxygenHandlerComponent* FindOxygenComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UOxygenHandlerComponent>() : nullptr); }
	
	UOxygenHandlerComponent(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void HandleLowValue(AActor* Instigator, AActor* Causer, const FGameplayEffectSpec* EffectSpec, float Magnitude, float OldValue, float NewValue) override;
	virtual void AfterASCInit() override;
};
