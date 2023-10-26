// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Executions/ArmorCalculation.h"
#include "AbilitySystem/Attributes/ArmorAttributeSet.h"

struct FArmorCalcStatics
{
	FGameplayEffectAttributeCaptureDefinition ArmorModifierDef;

	FArmorCalcStatics()
	{
		ArmorModifierDef = FGameplayEffectAttributeCaptureDefinition(UArmorAttributeSet::GetModifier_ArmorAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}
};

static FArmorCalcStatics& ArmorCalcStatics()
{
	static FArmorCalcStatics Statics;
	return Statics;
}

UArmorCalculation::UArmorCalculation()
{
	RelevantAttributesToCapture.Add(ArmorCalcStatics().ArmorModifierDef);
}

void UArmorCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
#if WITH_SERVER_CODE
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float ArmorModifier = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ArmorCalcStatics().ArmorModifierDef, EvaluateParameters, ArmorModifier);
	
	if (ArmorModifier != 0.0f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UArmorAttributeSet::GetModifier_ArmorAttribute(), EGameplayModOp::Additive, ArmorModifier));
	}
#endif // #if WITH_SERVER_CODE
}
