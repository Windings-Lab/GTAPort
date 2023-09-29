// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "WheeledVehiclePawn.h"
#include "AbilitySystem/LyraAbilitySet.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "Interaction/IInteractableTarget.h"
#include "Interfaces/Vehicle.h"
#include "GTAWheeledVehicle.generated.h"

class UVehicleExtensionComponent;
class ULyraCameraMode;
class ULyraCameraComponent;
class UCameraComponent;
class USpringArmComponent;
struct FInputActionValue;
class UInputMappingContext;
class ULyraInputConfig;
class ULyraAbilitySystemComponent;
class ULyraAbilitySet;
struct FInteractionOption;
/**
 * 
 */
UCLASS()
class GTAVEHICLESRUNTIME_API AGTAWheeledVehicle : public AWheeledVehiclePawn, public IInteractableTarget, public IVehicle
{
	GENERATED_BODY()

public:
	AGTAWheeledVehicle(const FObjectInitializer& ObjectInitializer);
	
	virtual void GatherInteractionOptions(const FInteractionQuery& InteractQuery, FInteractionOptionBuilder& OptionBuilder) override;
	virtual void CustomizeInteractionEventData(const FGameplayTag& InteractionEventTag, FGameplayEventData& InOutEventData) override;

protected:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnVehicleEnter(AActor* CarInstigator, ULyraAbilitySystemComponent* LyraASC);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnVehicleExit(AActor* CarInstigator, ULyraAbilitySystemComponent* LyraASC);

private:
	
	void Input_Move(const FInputActionValue& InputActionValue);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UVehicleExtensionComponent> VehicleExtensionComponent;
};
