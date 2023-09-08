// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/LyraCharacter.h"
#include "GTACharacter.generated.h"

class UGTACombatSet;
/**
 * 
 */
UCLASS()
class UKRAINEGTA_API AGTACharacter : public ALyraCharacter
{
	GENERATED_BODY()

public:
	AGTACharacter();

protected:
	virtual void OnAbilitySystemInitialized() override;
	virtual void OnAbilitySystemUninitialized() override;

private:
	void HandleLowStamina(AActor* StaminaInstigator, AActor* StaminaCauser, const FGameplayEffectSpec* Spec, float Magnitude, float OldValue, float NewValue);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="GTA|Abilities", Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayAbility> LowStaminaAbility;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GTA|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UGTAHeroComponent> GTAHeroComponent;
	
	UPROPERTY()
	TObjectPtr<const UGTACombatSet> GTACombatSet;
};
