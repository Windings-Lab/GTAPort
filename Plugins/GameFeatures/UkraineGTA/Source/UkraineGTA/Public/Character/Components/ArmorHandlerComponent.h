// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Abstract/AttributeHandlerComponent.h"
#include "ArmorHandlerComponent.generated.h"

class UArmorAttributeSet;
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

protected:
	virtual void AfterASCInit() override;
};
