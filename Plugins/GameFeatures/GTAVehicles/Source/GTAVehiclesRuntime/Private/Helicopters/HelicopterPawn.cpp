// Fill out your copyright notice in the Description page of Project Settings.


#include "Helicopters/HelicopterPawn.h"

#include "GTAVehicleGameplayTags.h"
#include "VehicleExtensionComponent.h"
#include "Camera/LyraCameraComponent.h"
#include "Camera/LyraCameraMode.h"
#include "Input/LyraInputComponent.h"
#include "Kismet/KismetSystemLibrary.h"

AHelicopterPawn::AHelicopterPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	HelicopterMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	HelicopterMesh->GetBodyInstance()->bSimulatePhysics = true;
	HelicopterMesh->GetBodyInstance()->bEnableGravity = false;
	RootComponent = HelicopterMesh;

	VehicleExtensionComponent = CreateDefaultSubobject<UVehicleExtensionComponent>(TEXT("VehicleExtensionComponent"));
	VehicleExtensionComponent->CameraComponent = CreateDefaultSubobject<ULyraCameraComponent>(TEXT("CameraComponent"));
	VehicleExtensionComponent->CameraComponent->SetupAttachment(RootComponent);

	MaxBladeRotationSpeed = 900.f;
	UpForce = 1000.f;
	TurnSpeed = 10.f;
	MaxSpeed = 3000.f;
}

void AHelicopterPawn::GatherInteractionOptions(const FInteractionQuery& InteractQuery,
	FInteractionOptionBuilder& OptionBuilder)
{
	OptionBuilder.AddInteractionOption(VehicleExtensionComponent->GetInteractionOption());
}

void AHelicopterPawn::CustomizeInteractionEventData(const FGameplayTag& InteractionEventTag,
	FGameplayEventData& InOutEventData)
{
	InOutEventData.OptionalObject = VehicleExtensionComponent;
}

void AHelicopterPawn::OnVehicleEnter_Implementation(AActor* CarInstigator, ULyraAbilitySystemComponent* LyraASC)
{
	if(ULyraInputComponent* LyraIC = Cast<ULyraInputComponent>(CarInstigator->InputComponent))
	{
		const ULyraInputConfig* InputConfig = VehicleExtensionComponent->GetInputConfig();
		if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(GTAVehicleGameplayTags::InputTag_Native_Vehicle_Thrust, false))
		{
			VehicleExtensionComponent->AddToNativeInputHandle(
				LyraIC->BindAction(IA, ETriggerEvent::Triggered, this, &ThisClass::Input_MoveUp).GetHandle());
		}

		if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(GTAVehicleGameplayTags::InputTag_Native_Vehicle_Yaw, false))
		{
			VehicleExtensionComponent->AddToNativeInputHandle(
				LyraIC->BindAction(IA, ETriggerEvent::Triggered, this, &ThisClass::Input_Yaw).GetHandle());
		}

		if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(GTAVehicleGameplayTags::InputTag_Native_Vehicle_Pitch, false))
		{
			VehicleExtensionComponent->AddToNativeInputHandle(
				LyraIC->BindAction(IA, ETriggerEvent::Triggered, this, &ThisClass::Input_Pitch).GetHandle());
		}

		if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(GTAVehicleGameplayTags::InputTag_Native_Vehicle_Roll, false))
		{
			VehicleExtensionComponent->AddToNativeInputHandle(
				LyraIC->BindAction(IA, ETriggerEvent::Triggered, this, &ThisClass::Input_Roll).GetHandle());
		}
	}
}

void AHelicopterPawn::OnVehicleExit_Implementation(AActor* CarInstigator, ULyraAbilitySystemComponent* LyraASC)
{
	Controller = nullptr;
}

void AHelicopterPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	UpdateBladeRotationSpeed(DeltaTime);
	UpdateForceVector(DeltaTime);
	
	const float CurrentTurnSpeed = GetTurnSpeed();
	UpdateYaw(CurrentTurnSpeed, DeltaTime);
	UpdatePitch(CurrentTurnSpeed, DeltaTime);
	UpdateRoll(CurrentTurnSpeed, DeltaTime);

	AddLinearForce(DeltaTime);
	HelicopterMesh->AddTorqueInDegrees(CurrentYaw, NAME_None, true);
	HelicopterMesh->AddTorqueInDegrees(CurrentPitch, NAME_None, true);
	HelicopterMesh->AddTorqueInDegrees(CurrentRoll, NAME_None, true);

	HelicopterMesh->SetPhysicsLinearVelocity(GetVelocity().GetClampedToMaxSize(MaxSpeed));
	
	PrintVariables();
}

void AHelicopterPawn::UpdateForceVector(float DT)
{
	FVector UpVector = GetActorUpVector();
	FVector Gravity;
	if(bBladesAtMaxSpeed)
		Gravity = FVector(0.f, 0.f, -30.f);
	else
		Gravity = FVector(0.f, 0.f, GetWorldSettings()->GetGravityZ());
	

	ForceVector = Gravity;
	ForceVector += GetUpForce() * UpVector * TargetThrottle;
}

void AHelicopterPawn::AddLinearForce(float DT)
{
	bool InputDetected = TargetThrottle || TargetRoll || TargetPitch;
	if(bBladesAtMaxSpeed && !InputDetected)
	{
		FVector TargetVelocity = FMath::VInterpTo(GetVelocity(), FVector::ZeroVector, DT, 0.5f);
		HelicopterMesh->SetPhysicsLinearVelocity(TargetVelocity);
	}
	
	ForceVector *= HelicopterMesh->GetMass();
	HelicopterMesh->AddForce(ForceVector);
}

void AHelicopterPawn::UpdateYaw(float CurrentTurnSpeed, float DT)
{
	CurrentYaw = GetActorUpVector() * (CurrentTurnSpeed * TargetYaw) * GetTurnSpeed();
}

void AHelicopterPawn::UpdatePitch(float CurrentTurnSpeed, float DT)
{
	FVector ForwardVector = GetActorForwardVector();
	
	CurrentPitchAngle = TargetPitch * 20.f + HelicopterMesh->GetComponentRotation().Pitch;
	CurrentPitchAngle = FMath::Clamp(CurrentPitchAngle, -20.f, 20.f);
	CurrentPitch = GetActorRightVector() * CurrentPitchAngle * CurrentTurnSpeed;
	
	ForceVector += GetUpForce() * ForwardVector * TargetPitch;
}

void AHelicopterPawn::UpdateRoll(float CurrentTurnSpeed, float DT)
{
	CurrentRollAngle = TargetRoll * 20.f + HelicopterMesh->GetComponentRotation().Roll;
	CurrentRollAngle = FMath::Clamp(CurrentRollAngle, -20.f, 20.f);
	CurrentRoll = GetActorForwardVector() * CurrentRollAngle * CurrentTurnSpeed;
	
	ForceVector += GetUpForce() * GetActorRightVector() * -TargetRoll;
}

float AHelicopterPawn::GetTurnSpeed()
{
	return TurnSpeed * bBladesAtMaxSpeed;
}

float AHelicopterPawn::GetUpForce()
{
	return UpForce * bBladesAtMaxSpeed;
}

void AHelicopterPawn::PrintVariables()
{
	if(!VehicleExtensionComponent->Entered()) return;

	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Blade Speed: %f"), CurrentBladeRotationSpeed)
	, true, false, FLinearColor::Green, 1.f, TEXT("CurrentBladeRotationSpeed"));

	FVector Acceleration = ForceVector / HelicopterMesh->GetMass();
	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Acceleration: %s"), *(Acceleration).ToString())
	, true, false, FLinearColor::Green, 1.f, TEXT("HeliAcceleration"));
	
	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("CurrentSpeed: %f"), GetVelocity().Size())
	, true, false, FLinearColor::Green, 1.f, TEXT("HeliSpeed"));
}

void AHelicopterPawn::Input_MoveUp(const FInputActionValue& InputActionValue)
{
	float InputValue = InputActionValue.Get<float>();

	TargetThrottle = InputValue;
}

void AHelicopterPawn::UpdateBladeRotationSpeed(float DT)
{
	float BladeAcceleration = VehicleExtensionComponent->Entered() ? 200.f : -200.f;
	CurrentBladeRotationSpeed += DT * BladeAcceleration;
	CurrentBladeRotationSpeed = FMath::Clamp(CurrentBladeRotationSpeed, 0.f, MaxBladeRotationSpeed);
	bBladesAtMaxSpeed = CurrentBladeRotationSpeed >= MaxBladeRotationSpeed;
	K2_OnRotateBlades(DT);
}

void AHelicopterPawn::Input_Yaw(const FInputActionValue& InputActionValue)
{
	float InputValue = InputActionValue.Get<float>();
	TargetYaw = InputValue;
}
void AHelicopterPawn::Input_Pitch(const FInputActionValue& InputActionValue)
{
	float InputValue = InputActionValue.Get<float>();
	TargetPitch = InputValue;
}
void AHelicopterPawn::Input_Roll(const FInputActionValue& InputActionValue)
{
	float Value = InputActionValue.Get<float>();
	TargetRoll = Value;
}

