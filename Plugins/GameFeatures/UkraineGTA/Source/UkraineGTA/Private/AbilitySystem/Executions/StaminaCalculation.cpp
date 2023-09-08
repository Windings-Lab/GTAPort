// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Executions/StaminaCalculation.h"

#include "AbilitySystem/Attributes/GTACombatSet.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

struct FStaminaCalcStatics
{
	FGameplayEffectAttributeCaptureDefinition StaminaModifierDef;

	FStaminaCalcStatics()
	{
		StaminaModifierDef = FGameplayEffectAttributeCaptureDefinition(UGTACombatSet::GetStaminaModifierAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}
};

static FStaminaCalcStatics& StaminaCalcStatics()
{
	static FStaminaCalcStatics Statics;
	return Statics;
}

UStaminaCalculation::UStaminaCalculation()
{
	RelevantAttributesToCapture.Add(StaminaCalcStatics().StaminaModifierDef);
}

void UStaminaCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
#if WITH_SERVER_CODE
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float StaminaModifier = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(StaminaCalcStatics().StaminaModifierDef, EvaluateParameters, StaminaModifier);

	ACharacter* Character = Cast<ACharacter>(ExecutionParams.GetSourceAbilitySystemComponent()->GetAvatarActor());
	bool RequieredSpeed = Character && Character->GetCharacterMovement()->Velocity.Length() >= 600.f;
	if(StaminaModifier < 0.f)
	{
		StaminaModifier = RequieredSpeed ? StaminaModifier : 0.f;
	}
	
	if (StaminaModifier != 0.0f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UGTACombatSet::GetStaminaModifierAttribute(), EGameplayModOp::Additive, StaminaModifier));
	}
#endif // #if WITH_SERVER_CODE
}
