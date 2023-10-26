// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Executions/OxygenCalculation.h"
#include "AbilitySystem/Attributes/OxygenAttributeSet.h"

struct FOxygenCalcStatics
{
	FGameplayEffectAttributeCaptureDefinition OxygenModifierDef;

	FOxygenCalcStatics()
	{
		OxygenModifierDef = FGameplayEffectAttributeCaptureDefinition(UOxygenAttributeSet::GetModifier_OxygenAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}
};

static FOxygenCalcStatics& OxygenCalcStatics()
{
	static FOxygenCalcStatics Statics;
	return Statics;
}

UOxygenCalculation::UOxygenCalculation()
{
	RelevantAttributesToCapture.Add(OxygenCalcStatics().OxygenModifierDef);
}

void UOxygenCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
#if WITH_SERVER_CODE
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float OxygenModifier = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(OxygenCalcStatics().OxygenModifierDef, EvaluateParameters, OxygenModifier);
	
	if (OxygenModifier != 0.0f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UOxygenAttributeSet::GetModifier_OxygenAttribute(), EGameplayModOp::Additive, OxygenModifier));
	}
#endif // #if WITH_SERVER_CODE
}
