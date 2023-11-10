// Fill out your copyright notice in the Description page of Project Settings.


#include "Bike/BicycleVehicle.h"

#include "ChaosVehicleMovementComponent.h"

ABicycleVehicle::ABicycleVehicle(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void ABicycleVehicle::Tick(float DeltaSeconds)
{
	auto* VehMovement = GetVehicleMovement();

	VehMovement->SetThrottleInput(MoveInput.Y);
	VehMovement->SetBrakeInput(MoveInput.Z);
	VehMovement->SetSteeringInput(MoveInput.X);

	Super::Tick(DeltaSeconds);
}
