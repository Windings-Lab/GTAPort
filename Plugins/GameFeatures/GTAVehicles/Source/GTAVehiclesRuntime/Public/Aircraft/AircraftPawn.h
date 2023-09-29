// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "Interaction/IInteractableTarget.h"
#include "Interfaces/Vehicle.h"
#include "AircraftPawn.generated.h"

struct FInputActionValue;
class ULyraCameraComponent;
class UVehicleExtensionComponent;
class ULyraCameraMode;

UCLASS()
class GTAVEHICLESRUNTIME_API AAircraftPawn : public APawn, public IInteractableTarget, public IVehicle
{
	GENERATED_BODY()

public:
	AAircraftPawn();

	virtual void GatherInteractionOptions(const FInteractionQuery& InteractQuery, FInteractionOptionBuilder& OptionBuilder) override;
	virtual void CustomizeInteractionEventData(const FGameplayTag& InteractionEventTag, FGameplayEventData& InOutEventData) override;

	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnUpdateYaw();

	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnUpdatePitch();
	
	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnUpdateRoll();

protected:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnVehicleEnter(AActor* CarInstigator, ULyraAbilitySystemComponent* LyraASC);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnVehicleExit(AActor* CarInstigator, ULyraAbilitySystemComponent* LyraASC);

	virtual void Tick(float DeltaSeconds) override;

private:
	void Input_Throttle(const FInputActionValue& InputActionValue);
	void Input_Yaw(const FInputActionValue& InputActionValue);
	void Input_Pitch(const FInputActionValue& InputActionValue);
	void Input_Roll(const FInputActionValue& InputActionValue);

	void UpdatePosition(float DeltaSeconds);
	
	void UpdateRotation(float DeltaSeconds);
	void UpdateYaw(float DeltaSeconds);
	void UpdatePitch(float DeltaSeconds);
	void UpdateRoll(float DeltaSeconds);
	
	void PrintVariables();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> AircraftMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UVehicleExtensionComponent> VehicleExtensionComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Constants")
	float MaxThrustSpeed;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Constants")
	float MinThrustToNotFall;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Constants")
	float ThrustMultiplier;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Constants")
	float Gravity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Constants")
	float Drag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Constants|Control Surfaces")
	float MaxFlapPitch;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Constants|Control Surfaces")
	float MaxElevatorPitch;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Constants|Control Surfaces")
	float MaxRudderYaw;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Constants|Control Surfaces")
	float MaxAileronPitch;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Constants|Rotation")
	float YawSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Constants|Rotation")
	float PitchSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Constants|Rotation")
	float RollSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dynamic Variables")
	float ThrustSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dynamic Variables")
	float CurrentSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dynamic Variables")
	float AppliedGravity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dynamic Variables|Rotation")
	float TargetYaw;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dynamic Variables|Rotation")
	float CurrentYaw;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dynamic Variables|Rotation")
	float TargetPitch;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dynamic Variables|Rotation")
	float CurrentPitch;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dynamic Variables|Rotation")
	float TargetRoll;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dynamic Variables|Rotation")
	float CurrentRoll;
};
