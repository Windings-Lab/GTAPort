// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Executions/GTADamageExecution.h"

#include "AbilitySystem/Attributes/ArmorAttributeSet.h"
#include "AbilitySystem/Attributes/LyraCombatSet.h"
#include "AbilitySystem/Attributes/LyraHealthSet.h"

struct FGTADamageStatics
{
	FGameplayEffectAttributeCaptureDefinition BaseDamageDef;
	FGameplayEffectAttributeCaptureDefinition ArmorModifierDef;

	FGTADamageStatics()
	{
		BaseDamageDef = FGameplayEffectAttributeCaptureDefinition(ULyraCombatSet::GetBaseDamageAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
		ArmorModifierDef = FGameplayEffectAttributeCaptureDefinition(UArmorAttributeSet::GetModifier_ArmorAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
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

	float Result = 0.f;
	float BaseDamage = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BaseDamageDef, EvaluateParameters, BaseDamage);
	BaseDamage = FMath::Max(BaseDamage, 0.0f);
	
	float ArmorReductionPercent = ExecutionParams.GetSourceAbilitySystemComponent()->GetNumericAttribute(UArmorAttributeSet::GetArmorPercentReductionAttribute());
	float CurrentArmorNormalized = ExecutionParams.GetSourceAbilitySystemComponent()->GetNumericAttribute(UArmorAttributeSet::GetArmorAttribute());
	CurrentArmorNormalized = CurrentArmorNormalized > 0.f ? 1.f : 0.f;
	
	float DamageToArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorModifierDef, EvaluateParameters, DamageToArmor);

	if(Spec.GetDuration() == FGameplayEffectConstants::INSTANT_APPLICATION)
	{
		Result = BaseDamage - (BaseDamage * (ArmorReductionPercent / 100.f)) * CurrentArmorNormalized;
	}
	else if(Spec.GetDuration() == FGameplayEffectConstants::INFINITE_DURATION)
	{
		Result = BaseDamage;
		DamageToArmor = 0.f;
	}
	
	if (Result > 0.0f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(ULyraHealthSet::GetDamageAttribute(), EGameplayModOp::Additive, Result));
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UArmorAttributeSet::GetModifier_ArmorAttribute(), EGameplayModOp::Additive, -DamageToArmor));
	}
#endif // #if WITH_SERVER_CODE
}
