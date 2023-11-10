// Fill out your copyright notice in the Description page of Project Settings.


#include "Car/CarVehicle.h"

#include "ChaosVehicleMovementComponent.h"

void ACarVehicle::Tick(float DeltaSeconds)
{
	auto* VehMovement = GetVehicleMovement();

	VehMovement->SetThrottleInput(MoveInput.Y);
	VehMovement->SetBrakeInput(MoveInput.Z);
	VehMovement->SetSteeringInput(MoveInput.X);
	
	Super::Tick(DeltaSeconds);
}
