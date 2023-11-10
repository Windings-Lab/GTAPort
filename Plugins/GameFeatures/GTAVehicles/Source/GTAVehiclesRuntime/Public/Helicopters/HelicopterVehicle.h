// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GTAVehicle.h"
#include "HelicopterVehicle.generated.h"

UCLASS()
class GTAVEHICLESRUNTIME_API AHelicopterVehicle : public AGTAVehicle
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AHelicopterVehicle(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void Tick(float DeltaTime) override;
	
private:
	void UpdateBladeRotationSpeed(float DT);
	void UpdateLinearForce(float DT);
	void AddLinearForce(float DT);
	
	void UpdateYaw(float CurrentTurnSpeed, float DT);
	void UpdatePitch(float CurrentTurnSpeed, float DT);
	void UpdateRoll(float CurrentTurnSpeed, float DT);
	
	float GetTurnSpeed() const;
	float GetUpForce() const;

	virtual bool PrintVariables() const override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dynamic Variables")
	float BladeRotationSpeed;
	
	// Acceleration
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dynamic Variables")
	FVector ForceVector;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dynamic Variables")
	FVector CurrentYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dynamic Variables")
	FVector CurrentPitch;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dynamic Variables")
	float CurrentPitchAngle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dynamic Variables")
	FVector CurrentRoll;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dynamic Variables")
	float CurrentRollAngle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Constants")
	float MaxBladeRotationSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Constants")
	float UpForce;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Constants")
	float TurnSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Constants")
	float MaxSpeed;
	
private:
	bool bBladesAtMaxSpeed;
};
