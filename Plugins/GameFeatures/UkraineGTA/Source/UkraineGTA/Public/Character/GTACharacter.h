// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Character/LyraCharacter.h"
#include "Interfaces/CanSwim.h"
#include "GTACharacter.generated.h"

class UGTACombatSet;
/**
 * 
 */
UCLASS()
class UKRAINEGTA_API AGTACharacter : public ALyraCharacter, public ICanSwim
{
	GENERATED_BODY()

public:
	AGTACharacter();

	//~WaterLogic Aggregation
	UFUNCTION(BlueprintCallable)
		virtual bool IsSwimming() const override;
	UFUNCTION(BlueprintCallable)
		virtual bool IsInWater() const override;
	UFUNCTION(BlueprintCallable)
		virtual bool IsTouchingGroundInWater() const override;
	UFUNCTION(BlueprintCallable)
		virtual bool CanSwimUp() const override;
	//~End of WaterLogic Aggregation

	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnSetSwimming(bool Value);

protected:
	virtual void OnAbilitySystemInitialized() override;
	virtual void OnAbilitySystemUninitialized() override;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent
		, AActor* OtherActor
		, UPrimitiveComponent* OtherComp
		, int32 OtherBodyIndex, bool bFromSweep
		, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent
		, AActor* OtherActor
		, UPrimitiveComponent* OtherComp
		, int32 OtherBodyIndex);

private:
	void HandleLowStamina(AActor* StaminaInstigator, AActor* StaminaCauser, const FGameplayEffectSpec* Spec, float Magnitude, float OldValue, float NewValue);
	void HandleLowHunger(AActor* HungerInstigator, AActor* HungerCauser, const FGameplayEffectSpec* Spec, float Magnitude, float OldValue, float NewValue);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GTA|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UGTAHeroComponent> GTAHeroComponent;
	
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<const UGTACombatSet> GTACombatSet;

	//~Water Logic
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GTA|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USphereComponent> SphereComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GTA|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UWaterLogicComponent> WaterLogicComponent;
	//~End of Water Logic
};
