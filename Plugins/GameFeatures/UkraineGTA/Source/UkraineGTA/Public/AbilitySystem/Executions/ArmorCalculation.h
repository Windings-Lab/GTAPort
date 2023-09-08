// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ArmorCalculation.generated.h"

/**
 * 
 */
UCLASS()
class UKRAINEGTA_API UArmorCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UArmorCalculation();

protected:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
	
};
