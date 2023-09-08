// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "StaminaCalculation.generated.h"

/**
 * 
 */
UCLASS()
class UKRAINEGTA_API UStaminaCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UStaminaCalculation();

protected:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
