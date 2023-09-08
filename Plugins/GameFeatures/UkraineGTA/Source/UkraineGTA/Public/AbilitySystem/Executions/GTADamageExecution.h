// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayEffectExecutionCalculation.h"
#include "GTADamageExecution.generated.h"

/**
 * 
 */
UCLASS()
class UKRAINEGTA_API UGTADamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UGTADamageExecution();

protected:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
