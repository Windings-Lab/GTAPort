// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GTACharacter.h"

#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/GTACombatSet.h"
#include "Character/GTAHeroComponent.h"

AGTACharacter::AGTACharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	GTAHeroComponent = CreateDefaultSubobject<UGTAHeroComponent>(TEXT("GTAHeroComponent"));
}

void AGTACharacter::OnAbilitySystemInitialized()
{
	Super::OnAbilitySystemInitialized();

	ULyraAbilitySystemComponent* LyraASC = GetLyraAbilitySystemComponent();

	GTACombatSet = LyraASC->GetSet<UGTACombatSet>();
	check(GTACombatSet)
	
	if (!GTACombatSet)
	{
		return;
	}

	GTACombatSet->OnLowStamina.AddUObject(this, &ThisClass::HandleLowStamina);
	GTACombatSet->OnLowHunger.AddUObject(this, &AGTACharacter::HandleLowHunger);
}

void AGTACharacter::OnAbilitySystemUninitialized()
{
	Super::OnAbilitySystemUninitialized();
}

void AGTACharacter::HandleLowStamina(AActor* StaminaInstigator, AActor* StaminaCauser, const FGameplayEffectSpec* Spec,
	float Magnitude, float OldValue, float NewValue)
{
	ULyraAbilitySystemComponent* LyraASC = GetLyraAbilitySystemComponent();
	{
		FGameplayEventData Payload;
		// TODO: Create and Define class of UkraineGTA GameplayTags
		Payload.EventTag = FGameplayTag::RequestGameplayTag(TEXT("UkraineGTA.Ability.Debuff.LowStamina"), true);
		Payload.Instigator = StaminaInstigator;
		Payload.Target = StaminaCauser;
		Payload.OptionalObject = Spec->Def;
		Payload.ContextHandle = Spec->GetEffectContext();
		Payload.InstigatorTags = *Spec->CapturedSourceTags.GetAggregatedTags();
		Payload.TargetTags = *Spec->CapturedTargetTags.GetAggregatedTags();
		Payload.EventMagnitude = Magnitude;

		FScopedPredictionWindow NewScopedWindow(LyraASC, true);
		LyraASC->HandleGameplayEvent(Payload.EventTag, &Payload);
	}
}

void AGTACharacter::HandleLowHunger(AActor* HungerInstigator, AActor* HungerCauser, const FGameplayEffectSpec* Spec,
	float Magnitude, float OldValue, float NewValue)
{
 	ULyraAbilitySystemComponent* LyraASC = GetLyraAbilitySystemComponent();
	{
		FGameplayEventData Payload;
		// TODO: Create and Define class of UkraineGTA GameplayTags
		Payload.EventTag = FGameplayTag::RequestGameplayTag(TEXT("UkraineGTA.Ability.Debuff.LowHunger"), true);
		Payload.Instigator = HungerInstigator;
		Payload.Target = HungerCauser;
		Payload.OptionalObject = Spec->Def;
		Payload.ContextHandle = Spec->GetEffectContext();
		Payload.InstigatorTags = *Spec->CapturedSourceTags.GetAggregatedTags();
		Payload.TargetTags = *Spec->CapturedTargetTags.GetAggregatedTags();
		Payload.EventMagnitude = Magnitude;

		FScopedPredictionWindow NewScopedWindow(LyraASC, true);
		LyraASC->HandleGameplayEvent(Payload.EventTag, &Payload);
	}
}
