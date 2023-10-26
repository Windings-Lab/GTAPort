// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "OxygenCalculation.generated.h"

/**
 * 
 */
UCLASS()
class UKRAINEGTA_API UOxygenCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	UOxygenCalculation();

protected:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
