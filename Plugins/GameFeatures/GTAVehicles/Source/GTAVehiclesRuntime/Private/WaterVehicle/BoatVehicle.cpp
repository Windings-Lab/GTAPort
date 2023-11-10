// Fill out your copyright notice in the Description page of Project Settings.


#include "WaterVehicle/BoatVehicle.h"

#include "ChaosVehicleMovementComponent.h"

// Sets default values
ABoatVehicle::ABoatVehicle(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GetVehicleMovementComponent()->DestroyComponent();
	GetVehicleMovementComponent()->SetActive(false);
}

void ABoatVehicle::Tick(float DeltaSeconds)
{
	GetMesh()->BodyInstance.AngularDamping = MoveInput.X == 0.f ? 1.f : 0.f;
	GetMesh()->BodyInstance.LinearDamping  = MoveInput.Y == 0.f ? 1.f : 0.f;

	FVector ForwardVector = GetActorForwardVector();
	ForwardVector.Z = 0.f;

	const FVector ForwardForce = ForwardVector * (MoveInput.Y * 1000.0f);
	const FVector RotationalTorque = FVector(0.0f, 0.f, MoveInput.X * 50.0f);
	
	GetMesh()->AddForce(ForwardForce, NAME_None, true);
	GetMesh()->AddTorqueInDegrees(RotationalTorque, NAME_None, true);
	
	Super::Tick(DeltaSeconds);
}
