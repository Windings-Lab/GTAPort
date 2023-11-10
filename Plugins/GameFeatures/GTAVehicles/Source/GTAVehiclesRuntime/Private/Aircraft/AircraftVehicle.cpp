// Fill out your copyright notice in the Description page of Project Settings.


#include "Aircraft/AircraftVehicle.h"

#include "ChaosVehicleMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

AAircraftVehicle::AAircraftVehicle(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GetVehicleMovementComponent()->DestroyComponent();
	GetVehicleMovementComponent()->SetActive(false);
	
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	
	GetMesh()->BodyInstance.bEnableGravity = false; // We are using our own gravity here
	
	Drag = 0.25f;
	Gravity = 981.f;
	ThrustMultiplier = 2500.f;
	MinThrustToNotFall = 4000.f;
	MaxThrustSpeed = 10000.f;
	
	MaxFlapPitch = 10.f;
	MaxElevatorPitch = 25.f;
	MaxRudderYaw = 45.f;
	MaxAileronPitch = 45.f;

	YawSpeed = 20.f;
	PitchSpeed = 50.f;
	RollSpeed = 50.f;

	ThrustSpeed = 0.f;
	CurrentSpeed = 0.f;
	AppliedGravity = 0.f;
	
	CurrentYaw = 0.f;
	CurrentPitch = 0.f;
	CurrentRoll = 0.f;
}

void AAircraftVehicle::Tick(float DeltaSeconds)
{
	ThrustSpeed = (MoveInput.Y * DeltaSeconds * ThrustMultiplier) + FMath::Clamp(ThrustSpeed, 0.f, MaxThrustSpeed);

	UpdatePosition(DeltaSeconds);
	UpdateRotation(DeltaSeconds);
	
	Super::Tick(DeltaSeconds);
}

void AAircraftVehicle::UpdatePosition(float DeltaSeconds)
{
	if(ThrustSpeed < CurrentSpeed)
	{
		CurrentSpeed = FMath::FInterpTo(CurrentSpeed, ThrustSpeed, DeltaSeconds, Drag);
	}
	else
	{
		CurrentSpeed = ThrustSpeed;
	}

	FVector NewPosition = GetActorForwardVector() * (CurrentSpeed);
	AppliedGravity = FMath::GetMappedRangeValueClamped(
		FVector2D(0.f, MinThrustToNotFall)
		, FVector2D(Gravity, 0.f)
		, CurrentSpeed);

	GetMesh()->AddForce(FVector(NewPosition.X, NewPosition.Y, NewPosition.Z - AppliedGravity)
		, NAME_None, true);
}

void AAircraftVehicle::UpdateRotation(float DeltaSeconds)
{
	UpdateYaw(DeltaSeconds);
	UpdatePitch(DeltaSeconds);
	UpdateRoll(DeltaSeconds);
}

void AAircraftVehicle::UpdateYaw(float DeltaSeconds)
{
	CurrentYaw = FMath::FInterpTo(CurrentYaw, YawInput, DeltaSeconds, 10.f);

	if(YawInput != 0.f)
	{
		FVector NewYaw = (CurrentYaw * YawSpeed) * GetActorUpVector();
		GetMesh()->AddTorqueInDegrees(NewYaw, NAME_None, true);
	}

	K2_OnUpdateYaw();
}

void AAircraftVehicle::UpdatePitch(float DeltaSeconds)
{
	CurrentPitch = FMath::FInterpTo(CurrentPitch, PitchInput, DeltaSeconds, 10.f);

	if(PitchInput != 0.f)
	{
		FVector NewPitch = (CurrentPitch * PitchSpeed) * GetActorRightVector();
		GetMesh()->AddTorqueInDegrees(NewPitch, NAME_None, true);
	}

	K2_OnUpdatePitch();
}

void AAircraftVehicle::UpdateRoll(float DeltaSeconds)
{
	CurrentRoll = FMath::FInterpTo(CurrentRoll, RollInput, DeltaSeconds, 10.f);

	if(RollInput != 0.f)
	{
		FVector NewRoll = (CurrentRoll * RollSpeed) * GetActorForwardVector();
		GetMesh()->AddTorqueInDegrees(NewRoll, NAME_None, true);
	}

	K2_OnUpdateRoll();
}

bool AAircraftVehicle::PrintVariables() const
{
	if(!Super::PrintVariables()) return false;
	
	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("ThrustSpeed: %f"), ThrustSpeed)
		, true, false, FLinearColor::Green, 1.f, TEXT("ThrustSpeed"));

	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("CurrentSpeed: %f"), CurrentSpeed)
	, true, false, FLinearColor::Green, 1.f, TEXT("CurrentSpeed"));

	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("AppliedGravity: %f"), AppliedGravity)
	, true, false, FLinearColor::Green, 1.f, TEXT("AppliedGravity"));

	return true;
}
