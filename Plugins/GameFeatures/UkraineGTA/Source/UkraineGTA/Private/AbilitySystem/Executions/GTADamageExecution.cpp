// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Executions/GTADamageExecution.h"

#include "AbilitySystem/Attributes/GTACombatSet.h"
#include "AbilitySystem/Attributes/LyraCombatSet.h"
#include "AbilitySystem/Attributes/LyraHealthSet.h"

struct FGTADamageStatics
{
	FGameplayEffectAttributeCaptureDefinition BaseDamageDef;
	FGameplayEffectAttributeCaptureDefinition ArmorPercentReductionDef;

	FGTADamageStatics()
	{
		BaseDamageDef = FGameplayEffectAttributeCaptureDefinition(ULyraCombatSet::GetBaseDamageAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
		ArmorPercentReductionDef = FGameplayEffectAttributeCaptureDefinition(UGTACombatSet::GetArmorPercentReductionAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}
};

static FGTADamageStatics& DamageStatics()
{
	static FGTADamageStatics Statics;
	return Statics;
}

UGTADamageExecution::UGTADamageExecution()
{
	RelevantAttributesToCapture.Add(DamageStatics().BaseDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPercentReductionDef);
}

void UGTADamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
#if WITH_SERVER_CODE
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float BaseDamage = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BaseDamageDef, EvaluateParameters, BaseDamage);
	BaseDamage = FMath::Max(BaseDamage, 0.0f);
	
	float ArmorReductionPercent = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeBaseValue(DamageStatics().ArmorPercentReductionDef, ArmorReductionPercent);

	float DamageResult = BaseDamage - (BaseDamage * (ArmorReductionPercent / 100.f));
	
	if (DamageResult > 0.0f)
	{
		// Apply a damage modifier, this gets turned into - health on the target
		
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(ULyraHealthSet::GetDamageAttribute(), EGameplayModOp::Additive, DamageResult));
	}
#endif // #if WITH_SERVER_CODE
}
