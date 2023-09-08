// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayEffectExecutionCalculation.h"
#include "GTAHealExecution.generated.h"

/**
 * 
 */
UCLASS()
class UKRAINEGTA_API UGTAHealExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UGTAHealExecution();

protected:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
