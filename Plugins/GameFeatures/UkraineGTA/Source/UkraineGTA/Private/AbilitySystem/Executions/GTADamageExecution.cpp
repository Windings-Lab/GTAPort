// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Executions/GTADamageExecution.h"

#include "AbilitySystem/Attributes/GTACombatSet.h"
#include "AbilitySystem/Attributes/LyraCombatSet.h"
#include "AbilitySystem/Attributes/LyraHealthSet.h"

struct FGTADamageStatics
{
	FGameplayEffectAttributeCaptureDefinition BaseDamageDef;
	FGameplayEffectAttributeCaptureDefinition ArmorModifierDef;

	FGTADamageStatics()
	{
		BaseDamageDef = FGameplayEffectAttributeCaptureDefinition(ULyraCombatSet::GetBaseDamageAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
		ArmorModifierDef = FGameplayEffectAttributeCaptureDefinition(UGTACombatSet::GetArmorModifierAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
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
	RelevantAttributesToCapture.Add(DamageStatics().ArmorModifierDef);
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
	
	float ArmorReductionPercent = ExecutionParams.GetSourceAbilitySystemComponent()->GetNumericAttribute(UGTACombatSet::GetArmorPercentReductionAttribute());
	float CurrentArmorNormalized = ExecutionParams.GetSourceAbilitySystemComponent()->GetNumericAttribute(UGTACombatSet::GetArmorAttribute());
	CurrentArmorNormalized = CurrentArmorNormalized > 0.f ? 1.f : 0.f;
	
	float DamageToArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorModifierDef, EvaluateParameters, DamageToArmor);

	float DamageResult = BaseDamage - (BaseDamage * (ArmorReductionPercent / 100.f)) * CurrentArmorNormalized;
	
	if (DamageResult > 0.0f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(ULyraHealthSet::GetDamageAttribute(), EGameplayModOp::Additive, DamageResult));
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UGTACombatSet::GetArmorModifierAttribute(), EGameplayModOp::Additive, -DamageToArmor));
	}
#endif // #if WITH_SERVER_CODE
}
