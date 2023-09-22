// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Character/LyraHeroComponent.h"
#include "GTAHeroComponent.generated.h"

/**
 * 
 */
UCLASS()
class UKRAINEGTA_API UGTAHeroComponent : public ULyraHeroComponent
{
	GENERATED_BODY()

public:
	UGTAHeroComponent(const FObjectInitializer& ObjectInitializer);

	/** Returns the hero component if one exists on the specified actor. */
	UFUNCTION(BlueprintPure, Category = "Lyra|Hero")
	static UGTAHeroComponent* FindGTAHeroComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UGTAHeroComponent>() : nullptr); }
	
protected:
	virtual void InitializePlayerInput(UInputComponent* PlayerInputComponent) override;
	
	void Input_Swim(const FInputActionValue& InputActionValue);
};
