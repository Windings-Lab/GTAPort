// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "WheeledVehiclePawn.h"
#include "AbilitySystem/LyraAbilitySet.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "Interaction/IInteractableTarget.h"
#include "GTAWheeledVehicle.generated.h"

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
class GTAVEHICLESRUNTIME_API AGTAWheeledVehicle : public AWheeledVehiclePawn, public IInteractableTarget
{
	GENERATED_BODY()

public:
	AGTAWheeledVehicle(const FObjectInitializer& ObjectInitializer);
	
	virtual void GatherInteractionOptions(const FInteractionQuery& InteractQuery, FInteractionOptionBuilder& OptionBuilder) override;

	UFUNCTION(BlueprintCallable)
	void OnCarEnter(AActor* CarInstigator, ULyraAbilitySystemComponent* LyraASC, ULyraAbilitySet* AbilitySetToGrant, ULyraInputConfig* InputConfig);
	UFUNCTION(BlueprintCallable)
	void OnCarExit(AActor* CarInstigator, ULyraAbilitySystemComponent* LyraASC);

private:
	TSubclassOf<ULyraCameraMode> DetermineCameraMode() const;
	
	void Input_Move(const FInputActionValue& InputActionValue);
	
	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

protected:
	
	UPROPERTY(EditAnywhere, Category="VehicleSettings")
	FInteractionOption Option;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TObjectPtr<ULyraCameraComponent> CameraComponent;

	// Default camera mode used by player controlled pawns.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Camera", meta=(AllowPrivateAccess))
	TSubclassOf<ULyraCameraMode> CarCameraMode;
	
	FLyraAbilitySet_GrantedHandles LoadedAbilitySetHandles;
	TArray<uint32> LoadedBindHandles;
	TObjectPtr<APawn> EnteredPawn;
};
