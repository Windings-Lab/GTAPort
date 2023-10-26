// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Executions/HungerCalculation.h"

#include "AbilitySystem/Attributes/HungerAttributeSet.h"

struct FHungerCalcStatics
{
	FGameplayEffectAttributeCaptureDefinition HungerModifierDef;

	FHungerCalcStatics()
	{
		HungerModifierDef = FGameplayEffectAttributeCaptureDefinition(UHungerAttributeSet::GetModifier_HungerAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}
};

static FHungerCalcStatics& HungerCalcStatics()
{
	static FHungerCalcStatics Statics;
	return Statics;
}

UHungerCalculation::UHungerCalculation()
{
	RelevantAttributesToCapture.Add(HungerCalcStatics().HungerModifierDef);
}

void UHungerCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                                FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
#if WITH_SERVER_CODE
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float Result = 0.f;
	float HungerModifier = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HungerCalcStatics().HungerModifierDef, EvaluateParameters, HungerModifier);
	Result = HungerModifier;

	if (Result != 0.0f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UHungerAttributeSet::GetModifier_HungerAttribute(), EGameplayModOp::Additive, Result));
	}
#endif // #if WITH_SERVER_CODE
}
