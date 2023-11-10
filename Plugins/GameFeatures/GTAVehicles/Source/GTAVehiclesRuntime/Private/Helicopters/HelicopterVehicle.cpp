// Fill out your copyright notice in the Description page of Project Settings.


#include "Helicopters/HelicopterVehicle.h"

#include "ChaosVehicleMovementComponent.h"
#include "VehicleExtensionComponent.h"
#include "Kismet/KismetSystemLibrary.h"

AHelicopterVehicle::AHelicopterVehicle(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GetMesh()->BodyInstance.bEnableGravity = false;

	GetVehicleMovementComponent()->DestroyComponent();
	GetVehicleMovementComponent()->SetActive(false);

	MaxBladeRotationSpeed = 900.f;
	UpForce = 1000.f;
	TurnSpeed = 10.f;
	MaxSpeed = 3000.f;
}

void AHelicopterVehicle::Tick(float DeltaTime)
{
	UpdateBladeRotationSpeed(DeltaTime);
	
	const float CurrentTurnSpeed = GetTurnSpeed();
	ForceVector = FVector::Zero();
	
	UpdateYaw(CurrentTurnSpeed, DeltaTime);
	UpdatePitch(CurrentTurnSpeed, DeltaTime);
	UpdateRoll(CurrentTurnSpeed, DeltaTime);
	UpdateLinearForce(DeltaTime);

	AddLinearForce(DeltaTime);
	GetMesh()->AddTorqueInDegrees(CurrentYaw, NAME_None, true);
	GetMesh()->AddTorqueInDegrees(CurrentPitch, NAME_None, true);
	GetMesh()->AddTorqueInDegrees(CurrentRoll, NAME_None, true);

	GetMesh()->SetPhysicsLinearVelocity(GetVelocity().GetClampedToMaxSize(MaxSpeed));
	
	Super::Tick(DeltaTime);
}

void AHelicopterVehicle::UpdateLinearForce(float DT)
{
	const FVector UpVector = GetActorUpVector();
	FVector Gravity;
	if(bBladesAtMaxSpeed)
		Gravity = FVector(0.f, 0.f, -30.f);
	else
		Gravity = FVector(0.f, 0.f, GetWorldSettings()->GetGravityZ());
	
	ForceVector += Gravity;
	ForceVector += GetUpForce() * UpVector * MoveInput.Z;
	ForceVector *= GetMesh()->GetMass();
}

void AHelicopterVehicle::AddLinearForce(float DT)
{
	if(bBladesAtMaxSpeed && !InputDetected())
	{
		const FVector TargetVelocity = FMath::VInterpTo(GetVelocity(), FVector::ZeroVector, DT, 0.5f);
		GetMesh()->SetPhysicsLinearVelocity(TargetVelocity);
	}
	
	GetMesh()->AddForce(ForceVector);
}

void AHelicopterVehicle::UpdateYaw(float CurrentTurnSpeed, float DT)
{
	CurrentYaw = GetActorUpVector() * (CurrentTurnSpeed * YawInput) * GetTurnSpeed();
}

void AHelicopterVehicle::UpdatePitch(float CurrentTurnSpeed, float DT)
{
	const FVector ForwardVector = GetActorForwardVector();
	
	CurrentPitchAngle = PitchInput * 20.f + GetMesh()->GetComponentRotation().Pitch;
	CurrentPitchAngle = FMath::Clamp(CurrentPitchAngle, -60.f, 60.f);
	CurrentPitch = GetActorRightVector() * CurrentPitchAngle * CurrentTurnSpeed;
	
	ForceVector += GetUpForce() * ForwardVector * PitchInput;
}

void AHelicopterVehicle::UpdateRoll(float CurrentTurnSpeed, float DT)
{
	CurrentRollAngle = RollInput * 20.f + GetMesh()->GetComponentRotation().Roll;
	CurrentRollAngle = FMath::Clamp(CurrentRollAngle, -60.f, 60.f);
	CurrentRoll = GetActorForwardVector() * CurrentRollAngle * CurrentTurnSpeed;
	
	ForceVector += GetUpForce() * GetActorRightVector() * -RollInput;
}

float AHelicopterVehicle::GetTurnSpeed() const
{
	return TurnSpeed * bBladesAtMaxSpeed;
}

float AHelicopterVehicle::GetUpForce() const
{
	return UpForce * bBladesAtMaxSpeed;
}

bool AHelicopterVehicle::PrintVariables() const
{
	if(!Super::PrintVariables()) return false;

	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Blade Speed: %f"), BladeRotationSpeed)
	, true, false, FLinearColor::Green, 1.f, TEXT("CurrentBladeRotationSpeed"));

	const FVector Acceleration = ForceVector / GetMesh()->GetMass();
	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Acceleration: %s"), *(Acceleration).ToString())
	, true, false, FLinearColor::Green, 1.f, TEXT("HeliAcceleration"));
	
	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("CurrentSpeed: %f"), GetVelocity().Size())
	, true, false, FLinearColor::Green, 1.f, TEXT("HeliSpeed"));

	return true;
}

void AHelicopterVehicle::UpdateBladeRotationSpeed(float DT)
{
	const float BladeAcceleration = VehicleExtensionComponent->DriverEntered() ? 200.f : -200.f;
	BladeRotationSpeed += DT * BladeAcceleration;
	BladeRotationSpeed = FMath::Clamp(BladeRotationSpeed, 0.f, MaxBladeRotationSpeed);
	bBladesAtMaxSpeed = BladeRotationSpeed >= MaxBladeRotationSpeed;
}

