// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GTAVehicle.h"
#include "AircraftVehicle.generated.h"

UCLASS()
class GTAVEHICLESRUNTIME_API AAircraftVehicle : public AGTAVehicle
{
	GENERATED_BODY()

public:
	AAircraftVehicle(const FObjectInitializer& ObjectInitializer);

	// TODO: Move implementation to animation blueprint
	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnUpdateYaw();

	// TODO: Move implementation to animation blueprint
	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnUpdatePitch();

	// TODO: Move implementation to animation blueprint
	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnUpdateRoll();

protected:
	virtual void Tick(float DeltaSeconds) override;

private:
	void UpdatePosition(float DeltaSeconds);
	
	void UpdateRotation(float DeltaSeconds);
	void UpdateYaw(float DeltaSeconds);
	void UpdatePitch(float DeltaSeconds);
	void UpdateRoll(float DeltaSeconds);
	
	virtual bool PrintVariables() const override;

protected:
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
	float CurrentYaw;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dynamic Variables|Rotation")
	float CurrentPitch;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dynamic Variables|Rotation")
	float CurrentRoll;
};
