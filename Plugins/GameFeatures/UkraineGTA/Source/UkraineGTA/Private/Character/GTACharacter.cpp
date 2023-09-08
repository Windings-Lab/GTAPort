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
}

void AGTACharacter::OnAbilitySystemUninitialized()
{
	Super::OnAbilitySystemUninitialized();
}

void AGTACharacter::HandleLowStamina(AActor* StaminaInstigator, AActor* StaminaCauser, const FGameplayEffectSpec* Spec,
	float Magnitude, float OldValue, float NewValue)
{
	ULyraAbilitySystemComponent* LyraASC = GetLyraAbilitySystemComponent();

	check(LowStaminaAbility)
	if(!LowStaminaAbility) return;
	LyraASC->TryActivateAbilityByClass(LowStaminaAbility, true);
}
