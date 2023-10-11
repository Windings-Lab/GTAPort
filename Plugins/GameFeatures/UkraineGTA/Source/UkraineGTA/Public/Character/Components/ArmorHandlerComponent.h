// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abstract/AttributeHandlerComponent.h"
#include "ArmorHandlerComponent.generated.h"

class UGTACombatSet;
/**
 * 
 */
UCLASS()
class UKRAINEGTA_API UArmorHandlerComponent : public UAttributeHandlerComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	static UArmorHandlerComponent* FindArmorComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UArmorHandlerComponent>() : nullptr); }
	
	UArmorHandlerComponent(const FObjectInitializer& ObjectInitializer);
	
	virtual void InitializeWithAbilitySystem(ULyraAbilitySystemComponent* InASC) override;
	virtual void UninitializeFromAbilitySystem() override;

	virtual float GetValue() const override;
	virtual float GetMaxValue() const override;
	
private:
	// Attribute set used by this component.
	UPROPERTY()
	TObjectPtr<const UGTACombatSet> GTACombatSet;
};
