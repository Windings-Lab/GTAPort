// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "Interaction/IInteractableTarget.h"
#include "Interfaces/Vehicle.h"
#include "HelicopterPawn.generated.h"

struct FInputActionValue;
class UVehicleExtensionComponent;
class ULyraCameraComponent;
class ULyraCameraMode;

UCLASS()
class GTAVEHICLESRUNTIME_API AHelicopterPawn : public APawn, public IInteractableTarget, public IVehicle
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AHelicopterPawn();

	virtual void GatherInteractionOptions(const FInteractionQuery& InteractQuery, FInteractionOptionBuilder& OptionBuilder) override;
	virtual void CustomizeInteractionEventData(const FGameplayTag& InteractionEventTag, FGameplayEventData& InOutEventData) override;

	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnRotateBlades(float DeltaTime);

protected:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnVehicleEnter(AActor* CarInstigator, ULyraAbilitySystemComponent* LyraASC);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnVehicleExit(AActor* CarInstigator, ULyraAbilitySystemComponent* LyraASC);

	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;
	
private:
	void Input_Throttle(const FInputActionValue& InputActionValue);
	float GetCurrentLift();
	void UpdatePreviousValues();
	
	void Input_Yaw(const FInputActionValue& InputActionValue);
	void Input_Pitch(const FInputActionValue& InputActionValue);
	void Input_Roll(const FInputActionValue& InputActionValue);

	void RotateBlades(float DeltaTime);
	float GetTurnSpeed();

	void PrintVariables();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> HelicopterMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UVehicleExtensionComponent> VehicleExtensionComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dynamic Variables")
	float TargetBladeRotationSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dynamic Variables")
	float CurrentBladeRotationSpeed;

	// Velocity
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dynamic Variables")
	FVector V;

	// Previous Velocity
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dynamic Variables")
	FVector Vp;

	// Acceleration
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dynamic Variables")
	FVector A;

	// Previous Acceleration
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dynamic Variables")
	FVector Ap;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dynamic Variables")
	FVector PreviousPosition;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dynamic Variables")
	float CurrentTurn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dynamic Variables")
	float CurrentPitch;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Constants")
	float MaxBladeRotationSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Constants")
	float ThrottleUpSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Constants")
	float TurnSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Constants")
	float BlurBladeSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Constants")
	TObjectPtr<UCurveFloat> LiftCurve;
};
