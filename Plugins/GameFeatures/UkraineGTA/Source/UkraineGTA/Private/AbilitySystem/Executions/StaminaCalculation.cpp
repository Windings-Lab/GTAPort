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

	float Result = 0.f;
	float StaminaModifier = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(StaminaCalcStatics().StaminaModifierDef, EvaluateParameters, StaminaModifier);

	const auto* Character = Cast<ACharacter>(ExecutionParams.GetSourceAbilitySystemComponent()->GetAvatarActor());
	check(Character)
	const auto* CharacterMovement = Character->GetCharacterMovement();
	const float Speed = CharacterMovement->Velocity.Length();
	const bool bWalking = CharacterMovement->MovementMode == MOVE_Walking;
	
	const float CurrentStamina = ExecutionParams.GetSourceAbilitySystemComponent()->GetNumericAttribute(UGTACombatSet::GetStaminaAttribute());
	const bool RequiredSpeed = CurrentStamina > 20.f ? Speed >= 600.f : Speed > 0.f;
	if(Spec.GetDuration() == FGameplayEffectConstants::INSTANT_APPLICATION)
	{
		Result = StaminaModifier;
	}
	else if(bWalking)
	{
		if(StaminaModifier < 0.f && RequiredSpeed)
		{
			Result = StaminaModifier;
		}
		else if(StaminaModifier > 0.f)
		{
			Result = StaminaModifier;
		}
	}
	
	if (Result != 0.0f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UGTACombatSet::GetStaminaModifierAttribute(), EGameplayModOp::Additive, Result));
	}
#endif // #if WITH_SERVER_CODE
}
